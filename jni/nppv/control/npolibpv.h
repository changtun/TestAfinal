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
#ifndef __NPO_LIBPV_H
#define __NPO_LIBPV_H

/*******************************************************/
/*              Includes				                   */
/*******************************************************/
#include "nporuntime.h"
/*******************************************************/
/*              Exported Defines and Macros            */
/*******************************************************/

#define USE_INSTALLATION	1
#define USE_DVB				1
#define	USE_HLS				1
#define	USE_EPG				1
#define USE_SYS				1
#define USE_PLAYER			1
#define USE_CMD				1
#define USE_TUNNER			1
#define USE_DB				1

/*******************************************************/
/*              Exported Types			                */
/*******************************************************/

class LibpvRootNPObject: public RuntimeNPObject
{
protected:
    friend class RuntimeNPClass < LibpvRootNPObject > ;
    LibpvRootNPObject(NPP instance, const NPClass *aClass): RuntimeNPObject
        (instance, aClass),
#if USE_INSTALLATION
        installationObj(NULL),
#endif
#if USE_DVB
        dvbObj(NULL),
#endif
#if USE_HLS
        hlsObj(NULL),
#endif
#if USE_EPG
        epgObj(NULL),
#endif
#if USE_SYS
        systemObj(NULL),
#endif
#if USE_PLAYER
        playerObj(NULL),
#endif
#if USE_CMD
        cmdObj(NULL),
#endif
#if USE_TUNNER
        tunerObj(NULL),
#endif
#if USE_DB
        dbObj(NULL)
#endif
    {}

    virtual ~LibpvRootNPObject();

    static const int propertyCount;
    static const NPUTF8 *const propertyNames[];

    InvokeResult getProperty(int index, NPVariant &result);

    static const int methodCount;
    static const NPUTF8 *const methodNames[];

    InvokeResult invoke(int index, const NPVariant *args, uint32_t argCount,
                        NPVariant &result);

private:
#if USE_INSTALLATION
    NPObject *installationObj;
#endif
#if USE_DVB
    NPObject *dvbObj;
#endif
#if USE_HLS
    NPObject *hlsObj;
#endif
#if USE_EPG
    NPObject *epgObj;
#endif
#if USE_SYS
    NPObject *systemObj;
#endif
#if USE_PLAYER
    NPObject *playerObj;
#endif
#if USE_CMD
    NPObject *cmdObj;
#endif
#if USE_TUNNER
    NPObject *tunerObj;
#endif
#if USE_DB
    NPObject *dbObj;
#endif
};


#if USE_INSTALLATION
class LibpvInstallationNPObject: public RuntimeNPObject
{
protected:
    LibpvInstallationNPObject(NPP instance, const NPClass *aClass): RuntimeNPObject(instance, aClass) {};
    virtual ~LibpvInstallationNPObject() {};

    friend class RuntimeNPClass < LibpvInstallationNPObject > ;
    static const int propertyCount;
    static const NPUTF8 *const propertyNames[];
    InvokeResult getProperty(int index, NPVariant &result);
    InvokeResult setProperty(int index, const NPVariant &value);

    static const int methodCount;
    static const NPUTF8 *const methodNames[];
    InvokeResult invoke(int index, const NPVariant *args, uint32_t argCount, NPVariant &result);

};
#endif


#if USE_DVB
class LibpvDvbNPObject: public RuntimeNPObject
{
protected:
    LibpvDvbNPObject(NPP instance, const NPClass *aClass): RuntimeNPObject (instance, aClass) {};
    virtual ~LibpvDvbNPObject() {};

    friend class RuntimeNPClass < LibpvDvbNPObject > ;
    static const int propertyCount;
    static const NPUTF8 *const propertyNames[];
    InvokeResult getProperty(int index, NPVariant &result);
    InvokeResult setProperty(int index, const NPVariant &value);
    static const int methodCount;
    static const NPUTF8 *const methodNames[];
    InvokeResult invoke(int index, const NPVariant *args, uint32_t argCount,	      NPVariant &result);

};
#endif

#if USE_HLS
class LibpvHlsNPObject: public RuntimeNPObject
{
protected:
    LibpvHlsNPObject(NPP instance, const NPClass *aClass): RuntimeNPObject(instance, aClass) {};
    virtual ~LibpvHlsNPObject() {};
    friend class RuntimeNPClass < LibpvHlsNPObject > ;
    static const int propertyCount;
    static const NPUTF8 *const propertyNames[];
    InvokeResult getProperty(int index, NPVariant &result);
    InvokeResult setProperty(int index, const NPVariant &value);
    static const int methodCount;
    static const NPUTF8 *const methodNames[];
    InvokeResult invoke(int index, const NPVariant *args, uint32_t argCount, NPVariant &result);
};
#endif





#if USE_EPG
class LibpvEpgNPObject: public RuntimeNPObject
{
protected:
    LibpvEpgNPObject(NPP instance, const NPClass *aClass): RuntimeNPObject (instance, aClass) {} ;
    virtual ~LibpvEpgNPObject() {} ;

    friend class RuntimeNPClass < LibpvEpgNPObject > ;
    static const int propertyCount;
    static const NPUTF8 *const propertyNames[];
    InvokeResult getProperty(int index, NPVariant &result);
    InvokeResult setProperty(int index, const NPVariant &value);
    static const int methodCount;
    static const NPUTF8 *const methodNames[];
    InvokeResult invoke(int index, const NPVariant *args, uint32_t argCount, NPVariant &result);

};
#endif




#if USE_SYS
class LibpvSysNPObject: public RuntimeNPObject
{
protected:
    LibpvSysNPObject(NPP instance, const NPClass *aClass): RuntimeNPObject (instance, aClass) {} ;
    virtual ~LibpvSysNPObject() {} ;

    friend class RuntimeNPClass < LibpvSysNPObject > ;
    static const int propertyCount;
    static const NPUTF8 *const propertyNames[];
    InvokeResult getProperty(int index, NPVariant &result);
    InvokeResult setProperty(int index, const NPVariant &value);
    static const int methodCount;
    static const NPUTF8 *const methodNames[];
    InvokeResult invoke(int index, const NPVariant *args, uint32_t argCount, NPVariant &result);

};

#endif




#if USE_PLAYER
class LibpvPlayerNPObject: public RuntimeNPObject
{
protected:
    LibpvPlayerNPObject(NPP instance, const NPClass *aClass): RuntimeNPObject (instance, aClass) {};
    virtual ~LibpvPlayerNPObject() {};
    friend class RuntimeNPClass < LibpvPlayerNPObject > ;
    static const int propertyCount;
    static const NPUTF8 *const propertyNames[];
    InvokeResult getProperty(int index, NPVariant &result);
    InvokeResult setProperty(int index, const NPVariant &value);
    static const int methodCount;
    static const NPUTF8 *const methodNames[];
    InvokeResult invoke(int index, const NPVariant *args, uint32_t argCount, NPVariant &result);

};
#endif




#if USE_CMD
class LibpvCmdNPObject: public RuntimeNPObject
{
protected:
    LibpvCmdNPObject(NPP instance, const NPClass *aClass): RuntimeNPObject (instance, aClass) {} ;
    virtual ~LibpvCmdNPObject() {};
    friend class RuntimeNPClass < LibpvCmdNPObject > ;
    static const int propertyCount;
    static const NPUTF8 *const propertyNames[];
    InvokeResult getProperty(int index, NPVariant &result);
    InvokeResult setProperty(int index, const NPVariant &value);
    static const int methodCount;
    static const NPUTF8 *const methodNames[];
    InvokeResult invoke(int index, const NPVariant *args, uint32_t argCount,      NPVariant &result);

};
#endif



#if USE_TUNNER
class LibpvTunerNPObject: public RuntimeNPObject
{
protected:
    LibpvTunerNPObject(NPP instance, const NPClass *aClass): RuntimeNPObject (instance, aClass) {};
    virtual ~LibpvTunerNPObject() {};
    friend class RuntimeNPClass < LibpvTunerNPObject > ;
    static const int propertyCount;
    static const NPUTF8 *const propertyNames[];
    InvokeResult getProperty(int index, NPVariant &result);
    InvokeResult setProperty(int index, const NPVariant &value);
    static const int methodCount;
    static const NPUTF8 *const methodNames[];
    InvokeResult invoke(int index, const NPVariant *args, uint32_t argCount, NPVariant &result);

};
#endif



#if USE_DB
class LibpvDbNPObject: public RuntimeNPObject
{
protected:
    LibpvDbNPObject(NPP instance, const NPClass *aClass): RuntimeNPObject(instance, aClass) {};
    virtual ~LibpvDbNPObject() {};
    friend class RuntimeNPClass < LibpvDbNPObject > ;
    static const int propertyCount;
    static const NPUTF8 *const propertyNames[];
    InvokeResult getProperty(int index, NPVariant &result);
    InvokeResult setProperty(int index, const NPVariant &value);
    static const int methodCount;
    static const NPUTF8 *const methodNames[];
    InvokeResult invoke(int index, const NPVariant *args, uint32_t argCount, NPVariant &result);

};
#endif


/*******************************************************/
/*              Exported Variables		                */
/*******************************************************/

/*******************************************************/
/*              External				                   */
/*******************************************************/

/*******************************************************/
/*              Exported Functions		                */
/*******************************************************/


#endif  /* #ifndef __NPOLIBPV_H */
/* End of stapi.h  --------------------------------------------------------- */





