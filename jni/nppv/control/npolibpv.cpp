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

#include "pvplugin.h"
#include "npolibpv.h"

#include "log/anpdebug.h"
#include "drv_avctrl.h" 






/*******************************************************/
/*               Private Defines and Macros			   */
/*******************************************************/
#define COUNTNAMES(a,b,c) const int a::b = sizeof(a::c)/sizeof(NPUTF8 *)


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

NPVariant copyNPVariant(const NPVariant &original)
{
    NPVariant res;

    if (NPVARIANT_IS_STRING(original))
        STRINGZ_TO_NPVARIANT(strdup(NPVARIANT_TO_STRING(original).UTF8Characters),
                             res);
    else if (NPVARIANT_IS_INT32(original))
        INT32_TO_NPVARIANT(NPVARIANT_TO_INT32(original), res);
    else if (NPVARIANT_IS_DOUBLE(original))
        DOUBLE_TO_NPVARIANT(NPVARIANT_TO_DOUBLE(original), res);
    else if (NPVARIANT_IS_OBJECT(original))
    {
        NPObject *obj = NPVARIANT_TO_OBJECT(original);
        NPN_RetainObject(obj);
        OBJECT_TO_NPVARIANT(obj, res);
    }
    else if (NPVARIANT_IS_BOOLEAN(original))
        BOOLEAN_TO_NPVARIANT(NPVARIANT_TO_BOOLEAN(original), res);

    return res;
}





LibpvRootNPObject::~LibpvRootNPObject()
{
    if (isValid())
    {
#if USE_INSTALLATION
        if (installationObj)
            NPN_ReleaseObject(installationObj);
#endif
#if USE_DVB
        if(dvbObj)
            NPN_ReleaseObject(dvbObj);
#endif
#if USE_HLS
        if(hlsObj)
            NPN_ReleaseObject(hlsObj);
#endif
#if USE_EPG
        if(epgObj)
            NPN_ReleaseObject(epgObj);
#endif
#if USE_SYS
        if(systemObj)
            NPN_ReleaseObject(systemObj);
#endif
#if USE_PLAYER
        if(playerObj)
            NPN_ReleaseObject(playerObj);
#endif
#if USE_CMD
        if(cmdObj)
            NPN_ReleaseObject(cmdObj);
#endif
#if USE_TUNNER
        if(tunerObj)
            NPN_ReleaseObject(tunerObj);
#endif
#if USE_DB
        if(dbObj)
            NPN_ReleaseObject(dbObj);
#endif
    }
}


const NPUTF8 *const LibpvRootNPObject::propertyNames[] =
{
    "installation",
    "dvb",
    "hls",
    "epg",
    "system",
    "player",
    "cmd",
    "tuner",
    "db",
    "VersionInfo"
};

COUNTNAMES(LibpvRootNPObject, propertyCount, propertyNames);

enum LibpvRootNPObjectPropertyIds
{
    ID_root_installation = 0,
    ID_root_dvb,
    ID_root_hls,
    ID_root_epg,
    ID_root_sys,
    ID_root_player,
    ID_root_cmd,
    ID_root_tuner,
    ID_root_db,
    ID_root_VersionInfo
};


RuntimeNPObject::InvokeResult LibpvRootNPObject::getProperty(int index, NPVariant &result)
{
    /* is plugin still running */
    if (isPluginRunning())
    {
        switch (index)
        {
        case ID_root_installation:
            InstantObj < LibpvInstallationNPObject > (installationObj);
            OBJECT_TO_NPVARIANT(NPN_RetainObject(installationObj), result);
            return INVOKERESULT_NO_ERROR;
        case ID_root_dvb:
            InstantObj < LibpvDvbNPObject > (dvbObj);
            OBJECT_TO_NPVARIANT(NPN_RetainObject(dvbObj), result);
            return INVOKERESULT_NO_ERROR;
        case ID_root_hls:
            InstantObj < LibpvHlsNPObject > (hlsObj);
            OBJECT_TO_NPVARIANT(NPN_RetainObject(hlsObj), result);
            return INVOKERESULT_NO_ERROR;
        case ID_root_epg:
            InstantObj < LibpvEpgNPObject > (epgObj);
            OBJECT_TO_NPVARIANT(NPN_RetainObject(epgObj), result);
            return INVOKERESULT_NO_ERROR;
        case ID_root_sys:
            InstantObj < LibpvSysNPObject > (systemObj);
            OBJECT_TO_NPVARIANT(NPN_RetainObject(systemObj), result);
            return INVOKERESULT_NO_ERROR;
        case ID_root_player:
            InstantObj < LibpvPlayerNPObject > (playerObj);
            OBJECT_TO_NPVARIANT(NPN_RetainObject(playerObj), result);
            return INVOKERESULT_NO_ERROR;
        case ID_root_cmd:
            InstantObj < LibpvCmdNPObject > (cmdObj);
            OBJECT_TO_NPVARIANT(NPN_RetainObject(cmdObj), result);
            return INVOKERESULT_NO_ERROR;
        case ID_root_tuner:
            InstantObj < LibpvTunerNPObject > (tunerObj);
            OBJECT_TO_NPVARIANT(NPN_RetainObject(tunerObj), result);
            return INVOKERESULT_NO_ERROR;
        case ID_root_db:
            InstantObj < LibpvDbNPObject > (dbObj);
            OBJECT_TO_NPVARIANT(NPN_RetainObject(dbObj), result);
            return INVOKERESULT_NO_ERROR;

        case ID_root_VersionInfo:
            return invokeResultString("pvware-1.0.0", result);
        default:
            ;
        }
    }
    return INVOKERESULT_GENERIC_ERROR;
}

const NPUTF8 *const LibpvRootNPObject::methodNames[] =
{
    "versionInfo"
};

COUNTNAMES(LibpvRootNPObject, methodCount, methodNames);

enum LibpvRootNPObjectMethodIds
{
    ID_root_versionInfo
};

RuntimeNPObject::InvokeResult LibpvRootNPObject::invoke(int index, const NPVariant *args, uint32_t argCount, NPVariant &result)
{
    if (!isPluginRunning())
        return INVOKERESULT_GENERIC_ERROR;

    switch (index)
    {
    case ID_root_versionInfo:
        if (0 != argCount)
            return INVOKERESULT_NO_SUCH_METHOD;
        return invokeResultString("pvware-1.0.0", result);
    }
    return INVOKERESULT_NO_SUCH_METHOD;
}


int StringConver(const char *psz, NPVariant &result)
{
    if (!psz)
        NULL_TO_NPVARIANT(result);
    else
    {
        size_t len = strlen(psz);
        NPUTF8 *retval = (NPUTF8 *)NPN_MemAlloc(len);
        if (!retval)
            return -1;
        else
        {
            memcpy(retval, psz, len);
            STRINGN_TO_NPVARIANT(retval, len, result);
        }
    }
    return 0;
}

#if USE_INSTALLATION

#include "npinstall.h"
static NPObject *installation_callbackObj;
void _npn_installation_cb(int search_type, char *str_buffer);

void _npn_installation_cb(int search_type, char *str_buffer)
{
    NPVariant result;
    NPVariant variant[2];

    INT32_TO_NPVARIANT(search_type, variant[0]);
    StringConver(str_buffer, variant[1]);
    NpInstallation_Retstr_Free();
    if(installation_callbackObj)
    {
        NPN_InvokeDefault(0, installation_callbackObj, variant, 2, &result);
        NPN_ReleaseVariantValue(&result);
    }
}



const NPUTF8 *const LibpvInstallationNPObject::propertyNames[] =
{
    "result_callback",
    "result_tptotal",
    "result_chtotal",
    "debug"
};
COUNTNAMES(LibpvInstallationNPObject, propertyCount, propertyNames);


enum LibpvInstallationNPObjectPropertyIds
{
    ID_installation_callback,
    ID_installation_tptotal,
    ID_installation_chtotal,
    ID_installation_debug
};

RuntimeNPObject::InvokeResult LibpvInstallationNPObject::getProperty(int index, NPVariant &result)
{
    if (isPluginRunning())
    {
        PvPluginBase *p_plugin = getPrivate < PvPluginBase > ();
        switch (index)
        {
        case ID_installation_tptotal:
        {
            int tp_total = NpInstallation_Get_Tptotal();
            INT32_TO_NPVARIANT(tp_total, result);
            return INVOKERESULT_NO_ERROR;
        }

        case ID_installation_chtotal:
        {
            int ch_total = NpInstallation_Get_Chtotal();
            INT32_TO_NPVARIANT(ch_total, result);
            return INVOKERESULT_NO_ERROR;
        }
        default:
            ;
        }
    }

    return INVOKERESULT_GENERIC_ERROR;
}

RuntimeNPObject::InvokeResult LibpvInstallationNPObject::setProperty(int index, const NPVariant &value)
{
    if (isPluginRunning())
    {
        switch (index)
        {
        case ID_installation_callback:
        {
            installation_callbackObj = NPN_RetainObject(NPVARIANT_TO_OBJECT(value));
            NpInstallation_Register(_npn_installation_cb);
            return INVOKERESULT_NO_ERROR;
        }
        case ID_installation_debug:
        {
            int val = 0;
			int debug_val = 0;
            if( !NPVARIANT_IS_STRING(value) )
                return INVOKERESULT_NO_SUCH_METHOD;

            if( NPVARIANT_IS_NULL(value) )
                return INVOKERESULT_NO_SUCH_METHOD;

            char *s = stringValue(NPVARIANT_TO_STRING(value));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;
			debug_val = atoi(s);
            val = NpInstallation_Debug(debug_val);
            free(s);
            return INVOKERESULT_NO_ERROR;
        }
        default:
            ;
        }
    }
    return INVOKERESULT_GENERIC_ERROR;
}

const NPUTF8 *const LibpvInstallationNPObject::methodNames[] =
{
    "init",
    "deinit",
    "start",
    "stop",
    "get_result",
    "free_result",
};

COUNTNAMES(LibpvInstallationNPObject, methodCount, methodNames);

enum LibpvInstallationNPObjectMethodIds
{
    ID_installation_init,
    ID_installation_deinit,
    ID_installation_start,
    ID_installation_stop,
    ID_installation_get_result,
    ID_installation_free_result,
};

RuntimeNPObject::InvokeResult LibpvInstallationNPObject::invoke(int index, const
        NPVariant *args, uint32_t argCount, NPVariant &result)
{
    if (isPluginRunning())
    {
        switch (index)
        {
        case ID_installation_init:
        {
            int ret = 0;
            if( 0 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
        case ID_installation_deinit:
        {
            int ret = 0;
            if( 0 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            ret = NpInstallation_Deinit();
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
        case ID_installation_start:
        {
            int ret = 0;
            if( 1 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( !NPVARIANT_IS_STRING(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;

            if( NPVARIANT_IS_NULL(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;

            char *s = stringValue(NPVARIANT_TO_STRING(args[0]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;

            ret = NpInstallation_Start(s);
            free(s);

            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
        case ID_installation_stop:
        {
            int ret = 0;
            if( 0 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            ret = NpInstallation_Stop();
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
        case ID_installation_get_result:
        {
            int ret = 0;
            if( 0 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            ret = NpInstallation_Get_SearchRet();
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
        case ID_installation_free_result:
        {
            int ret = 0;
            if( 0 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            ret = NpInstallation_Free_SearchRet();
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
        default:
            ;
        }
    }
    return INVOKERESULT_GENERIC_ERROR;
}
#endif

#if USE_DVB
#include "npdvb.h"

const NPUTF8 *const LibpvDvbNPObject::propertyNames[] =
{
    "debug",
};
COUNTNAMES(LibpvDvbNPObject, propertyCount, propertyNames);


enum LibpvDvbNPObjectPropertyIds
{
    ID_dvb_debug,
};

RuntimeNPObject::InvokeResult LibpvDvbNPObject::getProperty(int index, NPVariant &result)
{
    return INVOKERESULT_GENERIC_ERROR;
}

RuntimeNPObject::InvokeResult LibpvDvbNPObject::setProperty(int index, const NPVariant &value)
{
    if (isPluginRunning())
    {
        switch (index)
        {
        case ID_dvb_debug:
        {
            int val = 0;
			int debug_val = 0;
            if( !NPVARIANT_IS_STRING(value) )
                return INVOKERESULT_NO_SUCH_METHOD;

            if( NPVARIANT_IS_NULL(value) )
                return INVOKERESULT_NO_SUCH_METHOD;

            char *s = stringValue(NPVARIANT_TO_STRING(value));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;
			debug_val = atoi(s);
            val = NpDvb_Debug(debug_val);
            free(s);
            return INVOKERESULT_NO_ERROR;
        }
        default:
            ;
        }
    }
    return INVOKERESULT_GENERIC_ERROR;
}

const NPUTF8 *const LibpvDvbNPObject::methodNames[] =
{
	"init",
    "term",
    "play_c",
    "play_t",
    "stop",    
    "set_db",
    "dvb_vol",
    "set_area",
};

COUNTNAMES(LibpvDvbNPObject, methodCount, methodNames);

enum LibpvDvbNPObjectMethodIds
{
	ID_dvb_init,
    ID_dvb_term,
    ID_dvb_play_c,
    ID_dvb_play_t,
    ID_dvb_stop,    
    ID_dvb_setdb,
    ID_dvb_vol,
    ID_dvb_setarea,
};

RuntimeNPObject::InvokeResult LibpvDvbNPObject::invoke(int index, const
        NPVariant *args, uint32_t argCount, NPVariant &result)
{
    if (isPluginRunning())
    {
        DebugMessage("=========		LibpvDvbNPObject    invoke qzw add %d ", index);
        switch (index)
        {
        case ID_dvb_play_c:
        {
            int ret = 0;
            int freq_v = 0, symbol_v = 0, mod_v = 0, serv_id = 0, pmt_id = 0, vol_v = 0;

            if( 5 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;

            if( !NPVARIANT_IS_STRING(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( NPVARIANT_IS_NULL(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            char *s = stringValue(NPVARIANT_TO_STRING(args[0]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;
            freq_v = atoi(s);
            free(s);

            if( !NPVARIANT_IS_STRING(args[1]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( NPVARIANT_IS_NULL(args[1]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            s = stringValue(NPVARIANT_TO_STRING(args[1]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;
            symbol_v = atoi(s);
            free(s);

            if( !NPVARIANT_IS_STRING(args[2]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( NPVARIANT_IS_NULL(args[2]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            s = stringValue(NPVARIANT_TO_STRING(args[2]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;
            mod_v = atoi(s);
            free(s);

            if( !NPVARIANT_IS_STRING(args[3]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( NPVARIANT_IS_NULL(args[3]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            s = stringValue(NPVARIANT_TO_STRING(args[3]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;
            serv_id = atoi(s);
            free(s);

            if( !NPVARIANT_IS_STRING(args[4]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( NPVARIANT_IS_NULL(args[4]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            s = stringValue(NPVARIANT_TO_STRING(args[4]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;
            pmt_id = atoi(s);
            free(s);

            ret = NpDvb_Play_c(freq_v, symbol_v, mod_v, serv_id, pmt_id);
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
        break;

		case ID_dvb_play_t:
        {
            int ret = 0;
            int freq_v = 0, band_v = 0, serv_id = 0, pmt_id = 0, vol_v = 0;

            if( 4 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;

            if( !NPVARIANT_IS_STRING(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( NPVARIANT_IS_NULL(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            char *s = stringValue(NPVARIANT_TO_STRING(args[0]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;
            freq_v = atoi(s);
            free(s);

            if( !NPVARIANT_IS_STRING(args[1]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( NPVARIANT_IS_NULL(args[1]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            s = stringValue(NPVARIANT_TO_STRING(args[1]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;
            band_v = atoi(s);
            free(s);

            
            if( !NPVARIANT_IS_STRING(args[2]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( NPVARIANT_IS_NULL(args[2]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            s = stringValue(NPVARIANT_TO_STRING(args[2]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;
            serv_id = atoi(s);
            free(s);

            if( !NPVARIANT_IS_STRING(args[3]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( NPVARIANT_IS_NULL(args[3]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            s = stringValue(NPVARIANT_TO_STRING(args[3]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;
            pmt_id = atoi(s);
            free(s);

            ret = NpDvb_Play_t(freq_v, band_v, serv_id, pmt_id);
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
        break;

		
        case ID_dvb_stop:
        {
            int ret = 0;
            if( 0 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            ret = NpDvb_Stop();
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
        break;

        case ID_dvb_init:
        {
            int ret = 0;
            if( 0 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            ret = NpDvb_Init();
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
        break;

        case ID_dvb_term:
        {
            int ret = 0;
            DebugMessage("	pxwang	====		NpDvbPlay_term");
            if( 0 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            ret = NpDvb_term();
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
        break;
        case ID_dvb_vol:
        {
            int ret = 0;
            int vol_v = 0;

            DebugMessage("=========		ID_dvb_vol    ");

            if( 1 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;

            if( !NPVARIANT_IS_STRING(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( NPVARIANT_IS_NULL(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            char *s = stringValue(NPVARIANT_TO_STRING(args[0]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;
            vol_v = atoi(s);
            free(s);

            DebugMessage("=========		NpDvb_Set_volume   %d ", vol_v);
            ret = NpDvb_volume(vol_v);
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
        break;

        case ID_dvb_setdb:
        {
            int ret = 0;
            if( 1 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( !NPVARIANT_IS_STRING(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;

            if( NPVARIANT_IS_NULL(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;

            char *s = stringValue(NPVARIANT_TO_STRING(args[0]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;

            ret = NpDvb_DbPath(s);
            free(s);
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
        break;

		case ID_dvb_setarea:
        {
            int ret = 0;
            int x = 0, y = 0, w = 0, h = 0;

            if( 4 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;

            if( !NPVARIANT_IS_STRING(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( NPVARIANT_IS_NULL(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            char *s = stringValue(NPVARIANT_TO_STRING(args[0]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;
            x = atoi(s);
            free(s);

            if( !NPVARIANT_IS_STRING(args[1]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( NPVARIANT_IS_NULL(args[1]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            s = stringValue(NPVARIANT_TO_STRING(args[1]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;
            y = atoi(s);
            free(s);

            if( !NPVARIANT_IS_STRING(args[2]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( NPVARIANT_IS_NULL(args[2]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            s = stringValue(NPVARIANT_TO_STRING(args[2]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;
            w = atoi(s);
            free(s);

            if( !NPVARIANT_IS_STRING(args[3]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( NPVARIANT_IS_NULL(args[3]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            s = stringValue(NPVARIANT_TO_STRING(args[3]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;
            h = atoi(s);
            free(s);

            ret = NpDvb_Area(x, y, w, h);
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
		break;


        default:
            ;
        }
    }
    return INVOKERESULT_GENERIC_ERROR;
}

#endif


#if USE_HLS
#include "nphls.h"

static NPObject *hls_callbackObj;
void _npn_hls_cb(int search_type, char *str_buffer);

void _npn_hls_cb(int search_type, char *str_buffer)
{
    NPVariant result;
    NPVariant variant[2];

    INT32_TO_NPVARIANT(search_type, variant[0]);
    StringConver(str_buffer, variant[1]);
    if(hls_callbackObj)
    {
        NPN_InvokeDefault(0, hls_callbackObj, variant, 2, &result);
        NPN_ReleaseVariantValue(&result);
    }
}


const NPUTF8 *const LibpvHlsNPObject::propertyNames[] =
{
	"callback",
	"debug",
};
COUNTNAMES(LibpvHlsNPObject, propertyCount, propertyNames);


enum LibpvHlsNPObjectPropertyIds
{
	ID_hls_callback,
	ID_hls_debug,
};

RuntimeNPObject::InvokeResult LibpvHlsNPObject::getProperty(int index, NPVariant &result)
{
    return INVOKERESULT_GENERIC_ERROR;
}

RuntimeNPObject::InvokeResult LibpvHlsNPObject::setProperty(int index, const NPVariant &value)
{
	if (isPluginRunning())
	{
		switch (index)
		{
		case ID_hls_callback:
        {
            hls_callbackObj = NPN_RetainObject(NPVARIANT_TO_OBJECT(value));
            NpHls_Register(_npn_hls_cb);
            return INVOKERESULT_NO_ERROR;
        }
		case ID_hls_debug:
		{
			int val = 0;
			int debug_val = 0;
			if( !NPVARIANT_IS_STRING(value) )
				return INVOKERESULT_NO_SUCH_METHOD;

			if( NPVARIANT_IS_NULL(value) )
				return INVOKERESULT_NO_SUCH_METHOD;

			char *s = stringValue(NPVARIANT_TO_STRING(value));
			if( !s )
				return INVOKERESULT_OUT_OF_MEMORY;
			debug_val = atoi(s);
			val = NpHLS_Debug(debug_val);
			free(s);
			return INVOKERESULT_NO_ERROR;
		}
		default:
			;
		}
	}
	return INVOKERESULT_GENERIC_ERROR;
}


const NPUTF8 *const LibpvHlsNPObject::methodNames[] =
{
    "init",
    "term",
    "hlsplay",
    "stop",
    "hlsadd",
    "hlsnext",
};

COUNTNAMES(LibpvHlsNPObject, methodCount, methodNames);

enum LibpvHlsNPObjectMethodIds
{
    ID_hls_init,
    ID_hls_term,
    ID_hls_start,
    ID_hls_stop,
    ID_hls_add,
    ID_hls_seek_next,
};

RuntimeNPObject::InvokeResult LibpvHlsNPObject::invoke(int index, const NPVariant *args, uint32_t argCount, NPVariant &result)
{
	if (isPluginRunning())
	{
		switch (index)
		{
			case ID_hls_init:
			{
				int ret = 0;
				DebugMessage("	qzw add print ----------------------------\n");            
				if( 0 != argCount )
					return INVOKERESULT_NO_SUCH_METHOD;
				ret = NpHLS_Init();
				INT32_TO_NPVARIANT(ret, result);
				return INVOKERESULT_NO_ERROR;
			}
			break;

			case ID_hls_term:
			{
				int ret = 0;
				DebugMessage("	pxwang	====		ID_hls_term");
				if( 0 != argCount )
					return INVOKERESULT_NO_SUCH_METHOD;
				ret = NpHLS_Term();
				INT32_TO_NPVARIANT(ret, result);
				return INVOKERESULT_NO_ERROR;
			}
			break;

			case ID_hls_start:
			{
				int ret = 0;
				int type = 0;

				DebugMessage("	pxwang	====		ID_hls_start");
				if( 2 != argCount )
					return INVOKERESULT_NO_SUCH_METHOD;

				if( !NPVARIANT_IS_STRING(args[0]) )
					return INVOKERESULT_NO_SUCH_METHOD;
				if( NPVARIANT_IS_NULL(args[0]) )
					return INVOKERESULT_NO_SUCH_METHOD;
				char *s = stringValue(NPVARIANT_TO_STRING(args[0]));
				if( !s )
					return INVOKERESULT_OUT_OF_MEMORY;
				type = atoi(s);
				free(s);

				if( !NPVARIANT_IS_STRING(args[1]) )
					return INVOKERESULT_NO_SUCH_METHOD;
				if( NPVARIANT_IS_NULL(args[1]) )
					return INVOKERESULT_NO_SUCH_METHOD;
				s = stringValue(NPVARIANT_TO_STRING(args[1]));
				if( !s )
					return INVOKERESULT_OUT_OF_MEMORY;			

				ret = NpHLS_Start(type,s);
				free(s);
				INT32_TO_NPVARIANT(ret, result);
				return INVOKERESULT_NO_ERROR;
			}
			break;

			case ID_hls_stop:
			{
				int ret = 0;
				if( 0 != argCount )
					return INVOKERESULT_NO_SUCH_METHOD;
				ret = NpHLS_Stop();
				INT32_TO_NPVARIANT(ret, result);
				return INVOKERESULT_NO_ERROR;
			}
			break;

			case ID_hls_add:
			{
				int ret = 0;
				int type = 0;

				DebugMessage("	pxwang	====		ID_hls_add");
				if( 1 != argCount )
					return INVOKERESULT_NO_SUCH_METHOD;

				if( !NPVARIANT_IS_STRING(args[0]) )
					return INVOKERESULT_NO_SUCH_METHOD;
				if( NPVARIANT_IS_NULL(args[0]) )
					return INVOKERESULT_NO_SUCH_METHOD;
				char *s = stringValue(NPVARIANT_TO_STRING(args[0]));
				if( !s )
					return INVOKERESULT_OUT_OF_MEMORY;			

				ret = NpHLS_List_Add(s);
				free(s);
				INT32_TO_NPVARIANT(ret, result);
				return INVOKERESULT_NO_ERROR;
			}
			break;

			case ID_hls_seek_next:
			{
				int ret = 0;
				if( 0 != argCount )
					return INVOKERESULT_NO_SUCH_METHOD;
				ret = NpHLS_Seek_Next();
				INT32_TO_NPVARIANT(ret, result);
				return INVOKERESULT_NO_ERROR;
			}
			break;

			default:
			;
		}
	}
	return INVOKERESULT_GENERIC_ERROR;
}

#endif

#if USE_EPG
#include "npepg.h"

const NPUTF8 *const LibpvEpgNPObject::propertyNames[] =
{
	"debug",
};
COUNTNAMES(LibpvEpgNPObject, propertyCount, propertyNames);


enum LibpvEpgNPObjectPropertyIds
{
	ID_epg_debug,
};

RuntimeNPObject::InvokeResult LibpvEpgNPObject::getProperty(int index, NPVariant &result)
{
    return INVOKERESULT_GENERIC_ERROR;
}

RuntimeNPObject::InvokeResult LibpvEpgNPObject::setProperty(int index, const NPVariant &value)
{
	if (isPluginRunning())
	{
		switch (index)
		{
		case ID_epg_debug:
		{
			int val = 0;
			int debug_val = 0;
			if( !NPVARIANT_IS_STRING(value) )
				return INVOKERESULT_NO_SUCH_METHOD;

			if( NPVARIANT_IS_NULL(value) )
				return INVOKERESULT_NO_SUCH_METHOD;

			char *s = stringValue(NPVARIANT_TO_STRING(value));
			if( !s )
				return INVOKERESULT_OUT_OF_MEMORY;
			debug_val = atoi(s);
			val = NpEpg_Debug(debug_val);
			free(s);
			return INVOKERESULT_NO_ERROR;
		}
		default:
			;
		}
	}
	return INVOKERESULT_GENERIC_ERROR;
}


const NPUTF8 *const LibpvEpgNPObject::methodNames[] =
{
    "init",
    "term",
    "start",
    "stop",
    "pause",
    "resume",
    "getpf",
};

COUNTNAMES(LibpvEpgNPObject, methodCount, methodNames);

enum LibpvEpgNPObjectMethodIds
{
    ID_epg_init,
    ID_epg_term,
    ID_epg_start,
    ID_epg_stop,
    ID_epg_pause,
    ID_epg_resume,
    ID_epg_getpf,
};

RuntimeNPObject::InvokeResult LibpvEpgNPObject::invoke(int index, const NPVariant *args, uint32_t argCount, NPVariant &result)
{
    if (isPluginRunning())
    {
        switch (index)
        {
        case ID_epg_init:
        {
            int ret = 0;
            if( 0 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            ret = NpEpg_Init();
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
        break;
        case ID_epg_term:
        {
            int ret = 0;
            if( 0 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            ret = NpEpg_Term();
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
        break;
        case ID_epg_start:
        {
            int ret = 0;
            if( 0 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            ret = NpEpg_Start();
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
        break;

        case ID_epg_stop:
        {
            int ret = 0;
            if( 0 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            ret = NpEpg_Stop();
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
        break;

        case ID_epg_pause:
        {
            int ret = 0;
            if( 0 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            ret = NpEpg_Pause();
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
        break;

        case ID_epg_resume:
        {
            int ret = 0;
            if( 0 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            ret = NpEpg_Resume();
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
        break;

        case ID_epg_getpf:
        {
            char *ret = 0;
            int serv_id = 0, ts_id = 0, on_id = 0, flag = 0;

            if( 4 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;

            if( !NPVARIANT_IS_STRING(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( NPVARIANT_IS_NULL(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            char *s = stringValue(NPVARIANT_TO_STRING(args[0]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;
            serv_id = atoi(s);
            free(s);

            if( !NPVARIANT_IS_STRING(args[1]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( NPVARIANT_IS_NULL(args[1]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            s = stringValue(NPVARIANT_TO_STRING(args[1]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;
            ts_id = atoi(s);
            free(s);

            if( !NPVARIANT_IS_STRING(args[2]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( NPVARIANT_IS_NULL(args[2]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            s = stringValue(NPVARIANT_TO_STRING(args[2]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;
            on_id = atoi(s);
            free(s);

            if( !NPVARIANT_IS_STRING(args[3]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( NPVARIANT_IS_NULL(args[3]) )
                return INVOKERESULT_NO_SUCH_METHOD;
            s = stringValue(NPVARIANT_TO_STRING(args[3]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;
            flag = atoi(s);
            free(s);

            ret = NpEpg_GetPF(serv_id, ts_id, on_id, flag);
            invokeResultString(ret, result);
            NpEpg_Retstr_Free();
            return INVOKERESULT_NO_ERROR;
        }
        break;
        default:
            ;
        }
    }
    return INVOKERESULT_GENERIC_ERROR;
}

#endif


#if USE_SYS
#include "npsystem.h"

const NPUTF8 *const LibpvSysNPObject::propertyNames[] =
{
	"charset",
	"debug",
};
COUNTNAMES(LibpvSysNPObject, propertyCount, propertyNames);


enum LibpvSysNPObjectPropertyIds
{
	ID_sys_charset,
	ID_sys_debug,
};

RuntimeNPObject::InvokeResult LibpvSysNPObject::getProperty(int index, NPVariant &result)
{
    return INVOKERESULT_GENERIC_ERROR;
}

RuntimeNPObject::InvokeResult LibpvSysNPObject::setProperty(int index, const NPVariant &value)
{
	if (isPluginRunning())
	{
		switch (index)
		{
		case ID_sys_charset:
		{
			int val = 0;
			int debug_val = 0;
			if( !NPVARIANT_IS_STRING(value) )
				return INVOKERESULT_NO_SUCH_METHOD;

			if( NPVARIANT_IS_NULL(value) )
				return INVOKERESULT_NO_SUCH_METHOD;

			char *s = stringValue(NPVARIANT_TO_STRING(value));
			if( !s )
				return INVOKERESULT_OUT_OF_MEMORY;
			debug_val = atoi(s);
			val = NpSystem_set_charset(debug_val);
			free(s);
			return INVOKERESULT_NO_ERROR;
		}
		case ID_sys_debug:
		{
			int val = 0;
			int debug_val = 0;
			if( !NPVARIANT_IS_STRING(value) )
				return INVOKERESULT_NO_SUCH_METHOD;

			if( NPVARIANT_IS_NULL(value) )
				return INVOKERESULT_NO_SUCH_METHOD;

			char *s = stringValue(NPVARIANT_TO_STRING(value));
			if( !s )
				return INVOKERESULT_OUT_OF_MEMORY;
			debug_val = atoi(s);
			val = NpSystem_set_debug(debug_val);
			free(s);
			return INVOKERESULT_NO_ERROR;
		}
		default:
			;
		}
	}
	return INVOKERESULT_GENERIC_ERROR;
}


const NPUTF8 *const LibpvSysNPObject::methodNames[] =
{
};

COUNTNAMES(LibpvSysNPObject, methodCount, methodNames);

enum LibpvSysNPObjectMethodIds
{
};

RuntimeNPObject::InvokeResult LibpvSysNPObject::invoke(int index, const NPVariant *args, uint32_t argCount, NPVariant &result)
{

    return INVOKERESULT_GENERIC_ERROR;
}
#endif



#if USE_PLAYER
const NPUTF8 *const LibpvPlayerNPObject::propertyNames[] =
{
};
COUNTNAMES(LibpvPlayerNPObject, propertyCount, propertyNames);


enum LibpvPlayerNPObjectPropertyIds
{
};

RuntimeNPObject::InvokeResult LibpvPlayerNPObject::getProperty(int index, NPVariant &result)
{
    return INVOKERESULT_GENERIC_ERROR;
}

RuntimeNPObject::InvokeResult LibpvPlayerNPObject::setProperty(int index, const NPVariant &value)
{
    return INVOKERESULT_GENERIC_ERROR;
}

const NPUTF8 *const LibpvPlayerNPObject::methodNames[] =
{
};

COUNTNAMES(LibpvPlayerNPObject, methodCount, methodNames);

enum LibpvPlayerNPObjectMethodIds
{
};

RuntimeNPObject::InvokeResult LibpvPlayerNPObject::invoke(int index, const NPVariant *args, uint32_t argCount, NPVariant &result)
{
    return INVOKERESULT_GENERIC_ERROR;
}
#endif



#if USE_CMD
const NPUTF8 *const LibpvCmdNPObject::propertyNames[] =
{
};
COUNTNAMES(LibpvCmdNPObject, propertyCount, propertyNames);


enum LibpvCmdNPObjectPropertyIds
{
};

RuntimeNPObject::InvokeResult LibpvCmdNPObject::getProperty(int index, NPVariant &result)
{
    return INVOKERESULT_GENERIC_ERROR;
}

RuntimeNPObject::InvokeResult LibpvCmdNPObject::setProperty(int index, const NPVariant &value)
{
    return INVOKERESULT_GENERIC_ERROR;
}

const NPUTF8 *const LibpvCmdNPObject::methodNames[] =
{
};

COUNTNAMES(LibpvCmdNPObject, methodCount, methodNames);

enum LibpvCmdNPObjectMethodIds
{
};

RuntimeNPObject::InvokeResult LibpvCmdNPObject::invoke(int index, const NPVariant *args, uint32_t argCount, NPVariant &result)
{
    return INVOKERESULT_GENERIC_ERROR;
}
#endif



#if USE_TUNNER
const NPUTF8 *const LibpvTunerNPObject::propertyNames[] =
{
    "lock",
    "strength",
    "quality"
    "ber",
    "snr",
};
COUNTNAMES(LibpvTunerNPObject, propertyCount, propertyNames);


enum LibpvTunerNPObjectPropertyIds
{
    ID_tuner_lock,
    ID_tuner_strength,
    ID_tuner_quality,
    ID_tuner_ber,
    ID_tuner_snr,
};

RuntimeNPObject::InvokeResult LibpvTunerNPObject::getProperty(int index, NPVariant &result)
{
    if (isPluginRunning())
    {
        PvPluginBase *p_plugin = getPrivate < PvPluginBase > ();
        switch (index)
        {
        case ID_tuner_lock:
        {
            int lock_status = 1;
            INT32_TO_NPVARIANT(lock_status, result);
            return INVOKERESULT_NO_ERROR;
        }

        case ID_tuner_strength:
        {
            int strength_val = 95;
            INT32_TO_NPVARIANT(strength_val, result);
            return INVOKERESULT_NO_ERROR;
        }

        case ID_tuner_quality:
        {
            int quality_val = 79;
            INT32_TO_NPVARIANT(quality_val, result);
            return INVOKERESULT_NO_ERROR;
        }

        case ID_tuner_ber:
        {
            int ber_val = 33;
            INT32_TO_NPVARIANT(ber_val, result);
            return INVOKERESULT_NO_ERROR;
        }

        case ID_tuner_snr:
        {
            int snr_var = 66;
            INT32_TO_NPVARIANT(snr_var, result);
            return INVOKERESULT_NO_ERROR;
        }
        default:
            ;
        }
    }

    return INVOKERESULT_GENERIC_ERROR;
}

RuntimeNPObject::InvokeResult LibpvTunerNPObject::setProperty(int index, const NPVariant &value)
{
    return INVOKERESULT_GENERIC_ERROR;
}

const NPUTF8 *const LibpvTunerNPObject::methodNames[] =
{
    "SetFrequency",
};

COUNTNAMES(LibpvTunerNPObject, methodCount, methodNames);

enum LibpvTunerNPObjectMethodIds
{
    ID_tuner_set,
};

RuntimeNPObject::InvokeResult LibpvTunerNPObject::invoke(int index, const
        NPVariant *args, uint32_t argCount, NPVariant &result)
{
    if (isPluginRunning())
    {
        switch (index)
        {
        case ID_tuner_set:
        {
            int ret = 0;
            if( 1 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( !NPVARIANT_IS_STRING(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;

            if( NPVARIANT_IS_NULL(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;

            char *s = stringValue(NPVARIANT_TO_STRING(args[0]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;

            free(s);
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }
        default:
            ;
        }
    }
    return INVOKERESULT_GENERIC_ERROR;
}
#endif



#if USE_DB
#include "npdatabase.h"



const NPUTF8 *const LibpvDbNPObject::propertyNames[] =
{
    "debug",
};
COUNTNAMES(LibpvDbNPObject, propertyCount, propertyNames);


enum LibpvDbNPObjectPropertyIds
{
    ID_db_debug,
};

RuntimeNPObject::InvokeResult LibpvDbNPObject::getProperty(int index, NPVariant &result)
{
    return INVOKERESULT_GENERIC_ERROR;
}

RuntimeNPObject::InvokeResult LibpvDbNPObject::setProperty(int index, const NPVariant &value)
{
    if (isPluginRunning())
    {
        switch (index)
        {

        case ID_db_debug:
        {
            int val = 0;
            val = NPVARIANT_TO_INT32(value);
            NpDatabase_Debug(val);
            return INVOKERESULT_NO_ERROR;
        }
        default:
            ;
        }
    }
    return INVOKERESULT_GENERIC_ERROR;
}

const NPUTF8 *const LibpvDbNPObject::methodNames[] =
{
    "open",
    "close",
    "exe",
    "prepare",
    "step",
    "finallize",
    "column_int",
    "column_double",
    "column_text",
};

COUNTNAMES(LibpvDbNPObject, methodCount, methodNames);

enum LibpvDbNPObjectMethodIds
{
    ID_db_open,
    ID_db_close,
    ID_db_exe,
    ID_db_prepare,
    ID_db_step,
    ID_db_finallize,
    ID_db_column_int,
    ID_db_column_double,
    ID_db_column_text,
};

RuntimeNPObject::InvokeResult LibpvDbNPObject::invoke(int index, const
        NPVariant *args, uint32_t argCount, NPVariant &result)
{
    if (isPluginRunning())
    {
        switch (index)
        {
        case ID_db_open:
        {
            int ret = 0;
            if( 1 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( !NPVARIANT_IS_STRING(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;

            if( NPVARIANT_IS_NULL(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;

            char *s = stringValue(NPVARIANT_TO_STRING(args[0]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;

            ret = NpDatabase_Open(s);
            free(s);
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }

        case ID_db_close:
        {
            int ret = 0;
            if( 0 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            ret = NpDatabase_Close();
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }


        case ID_db_exe:
        {
            int ret = 0;
            if( 1 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( !NPVARIANT_IS_STRING(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;

            if( NPVARIANT_IS_NULL(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;

            char *s = stringValue(NPVARIANT_TO_STRING(args[0]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;

            ret = NpDatabase_Exe(s);
            free(s);
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }

        case ID_db_prepare:
        {
            int ret = 0;
            if( 1 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            if( !NPVARIANT_IS_STRING(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;

            if( NPVARIANT_IS_NULL(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;

            char *s = stringValue(NPVARIANT_TO_STRING(args[0]));
            if( !s )
                return INVOKERESULT_OUT_OF_MEMORY;

            ret = NpDatabase_Prepare(s);
            free(s);
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }

        case ID_db_step:
        {
            int ret = 0;
            if( 0 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            ret = NpDatabase_Step();
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }

        case ID_db_finallize:
        {
            int ret = 0;
            if( 0 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;
            ret = NpDatabase_Finalize();
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }


        case ID_db_column_int:
        {
            int ret = 0;
            if( 1 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;

            if( NPVARIANT_IS_NULL(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;

            int s = NPVARIANT_TO_INT32(args[0]);
            ret = NpDatabase_Column_Int(s);
            INT32_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }

        case ID_db_column_double:
        {
            double ret = 0;
            if( 1 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;

            if( NPVARIANT_IS_NULL(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;

            int s = NPVARIANT_TO_INT32(args[0]);
            ret = NpDatabase_Column_Double(s);

            DOUBLE_TO_NPVARIANT(ret, result);
            return INVOKERESULT_NO_ERROR;
        }

        case ID_db_column_text:
        {
            char *ret = 0;
            if( 1 != argCount )
                return INVOKERESULT_NO_SUCH_METHOD;

            if( NPVARIANT_IS_NULL(args[0]) )
                return INVOKERESULT_NO_SUCH_METHOD;

            int s = NPVARIANT_TO_INT32(args[0]);
            ret = NpDatabase_Column_Text(s);
            invokeResultString(ret, result);
            NpDatabase_Retstr_Free();
            return INVOKERESULT_NO_ERROR;
        }

        default:
            ;
        }
    }
    return INVOKERESULT_GENERIC_ERROR;
}
#endif


