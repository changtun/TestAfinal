#if 1

#include <stdlib.h>
#include "npinstall.h"

#include "installation.h"
#include "log/anpdebug.h"

#include "cJSON.h"


static int g_result_tptotal = 21;
static int g_result_chtotal = 22;
static npsearch_Notify_pfn g_search_cb;
static DVBCore_Tuner_Desc_u *main_tuner = NULL;
static DVBCore_Tuner_Desc_u *nor_tuner = NULL;

static int installation_bug = 1;
void installation_debug(char *str_temp)
{
    if(installation_bug == 0)
        return;
    DebugMessage("===   PXWANG	installation_debug	CallBy Function:%s", str_temp);
}

int NpInstallation_Debug(int flag)
{
	installation_bug = flag;
	return 0;
}


int NpInstallation_Init(void)
{
	installation_debug(__FUNCTION__);
    Installation_Init();

    return 11;
}


int NpInstallation_Register(npsearch_Notify_pfn result_cb)
{
	installation_debug(__FUNCTION__);
    g_search_cb = result_cb;
    return 11;
}

static char *install_instr = NULL;
static char *install_outstr = NULL;




void _npinstallation_result_cb(Install_Notify_Type_e eNotifyType, void *pData)
{
	installation_debug(__FUNCTION__);
    switch (eNotifyType)
    {
    case eINSTALL_COMPLETED:
    {
        Install_Search_Result_t tmp1;

        cJSON *root, *tp, *service, *fir, *sec, *thi;
        char *out;
        int i;
        int strLen;

        tmp1 = *(Install_Search_Result_t *)pData;
        root = cJSON_CreateObject();

        cJSON_AddNumberToObject(root, "tpNum", tmp1.u32TpNum);
        cJSON_AddItemToObject(root, "tp", tp = cJSON_CreateArray());
        for (i = 0; i < tmp1.u32TpNum; i++)
        {
            cJSON_AddItemToArray(tp, fir = cJSON_CreateObject());

            cJSON_AddNumberToObject(fir, "net_id", tmp1.ptTpList[i].u16NetId);
            cJSON_AddNumberToObject(fir, "ts_id", tmp1.ptTpList[i].u16TS_Id);
            cJSON_AddNumberToObject(fir, "on_id", tmp1.ptTpList[i].u16ON_Id);
            cJSON_AddItemToObject(fir, "ts", sec = cJSON_CreateObject());
            cJSON_AddNumberToObject(sec, "Souce", tmp1.ptTpList[i].uTunerDesc.eSignalSource);
            cJSON_AddItemToObject(sec, "u", thi = cJSON_CreateObject());

            switch(tmp1.ptTpList[i].uTunerDesc.eSignalSource)
            {
            case eDVBCORE_SIGNAL_SOURCE_DVBS:
            case eDVBCORE_SIGNAL_SOURCE_DVBS2:
                //cJSON_AddNumberToObject(thi, "t_id",tmp1.ptTpList[i].uTunerDesc.tSatellite.u8TunerId);
                //cJSON_AddNumberToObject(thi, "Fre", tmp1.ptTpList[i].uTunerDesc.tSatellite.u32Freq);
                //cJSON_AddNumberToObject(thi, "Sym", tmp1.ptTpList[i].uTunerDesc.tSatellite.u32SymRate);
                //cJSON_AddNumberToObject(thi, "Pol", tmp1.ptTpList[i].uTunerDesc.tSatellite.ePolarization);
                //cJSON_AddNumberToObject(thi, "Mod", tmp1.ptTpList[i].uTunerDesc.tSatellite.eMod);
                //cJSON_AddNumberToObject(thi, "FecIn", tmp1.ptTpList[i].uTunerDesc.tSatellite.eFEC_Inner);
                //cJSON_AddNumberToObject(thi, "Lnb", tmp1.ptTpList[i].uTunerDesc.tSatellite.eLnbType);
                //cJSON_AddNumberToObject(thi, "22K", tmp1.ptTpList[i].uTunerDesc.tSatellite.e22KHZFlag);
                //cJSON_AddNumberToObject(thi, "Diseqc", tmp1.ptTpList[i].uTunerDesc.tSatellite.ePortNumber);
                break;

            case eDVBCORE_SIGNAL_SOURCE_DVBC:
                cJSON_AddNumberToObject(thi, "t_id", tmp1.ptTpList[i].uTunerDesc.tCable.u8TunerId);
                cJSON_AddNumberToObject(thi, "Fre", tmp1.ptTpList[i].uTunerDesc.tCable.u32Freq);
                cJSON_AddNumberToObject(thi, "Sym", tmp1.ptTpList[i].uTunerDesc.tCable.u32SymbRate);
                cJSON_AddNumberToObject(thi, "Mod", tmp1.ptTpList[i].uTunerDesc.tCable.eMod);
                cJSON_AddNumberToObject(thi, "FecIn", tmp1.ptTpList[i].uTunerDesc.tCable.eFEC_Inner);
                cJSON_AddNumberToObject(thi, "FecOut", tmp1.ptTpList[i].uTunerDesc.tCable.eFEC_Outer);
                break;

            case eDVBCORE_SIGNAL_SOURCE_DVBT:
            case eDVBCORE_SIGNAL_SOURCE_DMBT:
                break;

            case eDVBCORE_SIGNAL_SOURCE_ANALOG:
                break;


            }
        }



        cJSON_AddNumberToObject(root, "servNum", tmp1.u32ServNum);
        cJSON_AddItemToObject(root, "serv", service = cJSON_CreateArray());
        for (i = 0; i < tmp1.u32ServNum; i++)
        {
            cJSON_AddItemToArray(service, fir = cJSON_CreateObject());
            cJSON_AddNumberToObject(fir, "serv_id", tmp1.ptServList[i].u16ServId);
            cJSON_AddNumberToObject(fir, "pmt_id", tmp1.ptServList[i].u16PMTPid);
            cJSON_AddNumberToObject(fir, "serv_type", tmp1.ptServList[i].u8ServType);
            cJSON_AddNumberToObject(fir, "ca_type", tmp1.ptServList[i].u8FreeCA_Mode);
            cJSON_AddNumberToObject(fir, "name_type", tmp1.ptServList[i].uServNameType);
            cJSON_AddStringToObject(fir, "ch_name", tmp1.ptServList[i].sServName);
        }


        out = cJSON_Print(root);
        cJSON_Delete(root);

        if(install_outstr != NULL)
        {
            free(install_outstr);
            install_outstr = NULL;
        }

        strLen = strlen(out);
        if(strLen > 0)
        {
            install_outstr = (char *)malloc(strLen + 10);
            if(install_outstr == NULL)
            {
                return install_outstr;
            }
            sprintf( install_outstr, "%s", out);
        }

        free(out);
        g_search_cb(eNotifyType, install_outstr);
    }
    break;

    case eINSTALL_INTERRUPT:
    {
        g_search_cb(eNotifyType, "eINSTALL_INTERRUPT");
    }
    break;
    case eINSTALL_FAILED:
    {
        g_search_cb(eNotifyType, "eINSTALL_FAILED");
    }
    break;
    case eINSTALL_PROGRESS:
    {
        Install_Progress_t tmp;
        tmp = *(Install_Progress_t *) pData;

        cJSON *root, *curTP, *fir, *sec, *thi;
        char *out;
        int strLen;

        root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, "progress", tmp.u32Progress);

        cJSON_AddItemToObject(root, "curTP", fir = cJSON_CreateObject());

        cJSON_AddItemToObject(fir, "ts", sec = cJSON_CreateObject());
        cJSON_AddNumberToObject(sec, "Souce", tmp.puCurrTuner->eSignalSource);
        cJSON_AddItemToObject(sec, "u", thi = cJSON_CreateObject());

        switch(tmp.puCurrTuner->eSignalSource)
        {
        case eDVBCORE_SIGNAL_SOURCE_DVBS:
        case eDVBCORE_SIGNAL_SOURCE_DVBS2:
            //cJSON_AddNumberToObject(thi, "t_id",tmp.puCurrTuner->tSatellite.u8TunerId);
            //cJSON_AddNumberToObject(thi, "Fre", tmp.puCurrTuner->tSatellite.u32Freq);
            //cJSON_AddNumberToObject(thi, "Sym", tmp.puCurrTuner->tSatellite.u32SymRate);
            //cJSON_AddNumberToObject(thi, "Pol", tmp.puCurrTuner->tSatellite.ePolarization);
            //cJSON_AddNumberToObject(thi, "Mod", tmp.puCurrTuner->tSatellite.eMod);
            //cJSON_AddNumberToObject(thi, "FecIn", tmp.puCurrTuner->tSatellite.eFEC_Inner);
            //cJSON_AddNumberToObject(thi, "Lnb", tmp.puCurrTuner->tSatellite.eLnbType);
            //cJSON_AddNumberToObject(thi, "22K", tmp.puCurrTuner->tSatellite.e22KHZFlag);
            //cJSON_AddNumberToObject(thi, "Diseqc", tmp.puCurrTuner->tSatellite.ePortNumber);
            break;

        case eDVBCORE_SIGNAL_SOURCE_DVBC:
            cJSON_AddNumberToObject(thi, "t_id", tmp.puCurrTuner->tCable.u8TunerId);
            cJSON_AddNumberToObject(thi, "Fre", tmp.puCurrTuner->tCable.u32Freq);
            cJSON_AddNumberToObject(thi, "Sym", tmp.puCurrTuner->tCable.u32SymbRate);
            cJSON_AddNumberToObject(thi, "Mod", tmp.puCurrTuner->tCable.eMod);
            break;

        case eDVBCORE_SIGNAL_SOURCE_DVBT:
        case eDVBCORE_SIGNAL_SOURCE_DMBT:
            break;

        case eDVBCORE_SIGNAL_SOURCE_ANALOG:
            break;
        }

        out = cJSON_Print(root);
        cJSON_Delete(root);

        if(install_outstr != NULL)
        {
            free(install_outstr);
            install_outstr = NULL;
        }

        strLen = strlen(out);
        if(strLen > 0)
        {
            install_outstr = (char *)malloc(strLen + 10);
            if(install_outstr == NULL)
            {
                return install_outstr;
            }
            sprintf( install_outstr, "%s", out);
        }
        free(out);

        g_search_cb(eNotifyType, install_outstr);
    }
    break;
    case eINSTALL_TP_COMPLETED:
    {
        g_search_cb(eNotifyType, "eINSTALL_TP_COMPLETED");
    }
    break;
    case eINSTALL_TP_FAILED:
    {
        g_search_cb(eNotifyType, "eINSTALL_TP_FAILED");
    }
    break;
    case eINSTALL_OTHER:
    {
        g_search_cb(eNotifyType, "eINSTALL_OTHER");
    }
    break;
    }
}



int NpInstallation_Deinit(void)
{
    installation_debug(__FUNCTION__);

    Installation_Deinit();
    if(main_tuner)
    {
        free(main_tuner);
    }

    if(nor_tuner)
    {
        free(nor_tuner);
    }
    return 12;
}



int NpInstallation_Start(char *params_str)
{

    Install_Cfg_t tCfg;
    Install_Timeout_t tTimeout;

    int i = 0, count = 0;;
    cJSON *root, *timeout, *tp, *ts, *this, *s_type, *num;
    int get_val, cou_val;
    char *out;


    installation_debug(__FUNCTION__);
    tCfg.pfnNotifyFunc = _npinstallation_result_cb;

    root = cJSON_Parse(params_str);
    if (!root)
    {
        return 1;
    }

    s_type = cJSON_GetObjectItem(root, "search_type");
    if(!s_type)
    {
        return 2;
    }

    tCfg.eSearchType = s_type->valueint;


    timeout = cJSON_GetObjectItem(root, "to");
    if(!timeout)
    {
        tCfg.ptTimeout = NULL;
    }
    else
    {
        tTimeout.u32TunerTimeout = cJSON_GetObjectItem(timeout, "Tu")->valueint;
        tTimeout.u32NITActualTimeout = cJSON_GetObjectItem(timeout, "NAct")->valueint;
        tTimeout.u32NITOtherTimeout = cJSON_GetObjectItem(timeout, "NOth")->valueint;
        tTimeout.u32BATTimeout = cJSON_GetObjectItem(timeout, "BAT")->valueint;
        tTimeout.u32PATTimeout = cJSON_GetObjectItem(timeout, "PAT")->valueint;
        tTimeout.u32PMTTimeout = cJSON_GetObjectItem(timeout, "PMT")->valueint;
        tTimeout.u32SDTActualTimeout = cJSON_GetObjectItem(timeout, "SAct")->valueint;
        tTimeout.u32SDTOtherTimeout = cJSON_GetObjectItem(timeout, "SOth")->valueint;
        tCfg.ptTimeout = &tTimeout;
    }


    num = cJSON_GetObjectItem(root, "matpNum");
    if(!num)
    {
        get_val = 0;
    }
    else
    {
        get_val = num ->valueint;
    }

    tp = cJSON_GetObjectItem(root, "matp");
    if(!tp)
    {
        tCfg.u32MainTpNum = 0;
        tCfg.puMainTpDescList = NULL;
    }
    else
    {
        cou_val = cJSON_GetArraySize(tp);
        count = min(get_val, cou_val);
        if(count > 0)
        {
            if(main_tuner)
                free(main_tuner);
            main_tuner = malloc(count * sizeof(DVBCore_Tuner_Desc_u));
            if(main_tuner == NULL)
            {
                return 3;
            }

            for(i = 0; i < count; i++)
            {
                ts = cJSON_GetArrayItem(tp, i);
                if(ts)
                {
                    main_tuner[i].tCable.eSignalSource = cJSON_GetObjectItem(ts, "Souce")->valueint;
                    this = cJSON_GetObjectItem(ts, "u");
                    if(this)
                    {
                        switch(main_tuner[i].tCable.eSignalSource)
                        {
                        case eDVBCORE_SIGNAL_SOURCE_DVBS:
                        case eDVBCORE_SIGNAL_SOURCE_DVBS2:
                            //main_tuner[i].tSatellite.u8TunerId = cJSON_GetObjectItem(this,"t_id")->valueint;
                            //main_tuner[i].tSatellite.u32Freq = cJSON_GetObjectItem(this,"Fre")->valueint;
                            //main_tuner[i].tSatellite.u32SymRate = cJSON_GetObjectItem(this,"Sym")->valueint;
                            //main_tuner[i].tSatellite.ePolarization = cJSON_GetObjectItem(this,"Pol")->valueint;
                            //main_tuner[i].tSatellite.eLnbType = cJSON_GetObjectItem(this,"Lnb")->valueint;
                            //main_tuner[i].tSatellite.e22KHZFlag = cJSON_GetObjectItem(this,"22K")->valueint;
                            //main_tuner[i].tSatellite.ePortNumber = cJSON_GetObjectItem(this,"Diseqc")->valueint;
                            break;

                        case eDVBCORE_SIGNAL_SOURCE_DVBC:
                            main_tuner[i].tCable.u8TunerId = cJSON_GetObjectItem(this, "t_id")->valueint;
                            main_tuner[i].tCable.u32Freq = cJSON_GetObjectItem(this, "Fre")->valueint;
                            main_tuner[i].tCable.u32SymbRate = cJSON_GetObjectItem(this, "Sym")->valueint;
                            main_tuner[i].tCable.eMod = cJSON_GetObjectItem(this, "Mod")->valueint;

                            break;

                        case eDVBCORE_SIGNAL_SOURCE_DVBT:
                        case eDVBCORE_SIGNAL_SOURCE_DMBT:
                            break;

                        case eDVBCORE_SIGNAL_SOURCE_ANALOG:
                            break;


                        }
                    }

                }
                else
                {
                    tCfg.u32MainTpNum = 0;
                    tCfg.puMainTpDescList = NULL;
                }
            }
            tCfg.u32MainTpNum = count;
            tCfg.puMainTpDescList = main_tuner;
        }

    }

    num = cJSON_GetObjectItem(root, "nortpNum");
    if(!num)
    {
        get_val = 0;
    }
    else
    {
        get_val = num ->valueint;
    }
    tp = cJSON_GetObjectItem(root, "nortp");
    if(!tp)
    {
        tCfg.u32NormalTpNum = 0;
        tCfg.puNormalTpDescList = NULL;
    }
    else
    {
        cou_val = cJSON_GetArraySize(tp);
        count = min(get_val, cou_val);
        if(count > 0)
        {
            if(nor_tuner)
                free(nor_tuner);
            nor_tuner = malloc(count * sizeof(DVBCore_Tuner_Desc_u));
            if(nor_tuner == NULL)
            {
                return 3;
            }



            for(i = 0; i < count; i++)
            {
                ts = cJSON_GetArrayItem(tp, i);
                if(ts)
                {
                    nor_tuner[i].tCable.eSignalSource = cJSON_GetObjectItem(ts, "Souce")->valueint;
                    this = cJSON_GetObjectItem(ts, "u");
                    if(this)
                    {
                        switch(nor_tuner[i].tCable.eSignalSource)
                        {
                        case eDVBCORE_SIGNAL_SOURCE_DVBS:
                        case eDVBCORE_SIGNAL_SOURCE_DVBS2:
                            //nor_tuner[i].tSatellite.u8TunerId = cJSON_GetObjectItem(this,"t_id")->valueint;
                            //nor_tuner[i].tSatellite.u32Freq = cJSON_GetObjectItem(this,"Fre")->valueint;
                            //nor_tuner[i].tSatellite.u32SymRate = cJSON_GetObjectItem(this,"Sym")->valueint;
                            //nor_tuner[i].tSatellite.ePolarization = cJSON_GetObjectItem(this,"Pol")->valueint;
                            //nor_tuner[i].tSatellite.eLnbType = cJSON_GetObjectItem(this,"Lnb")->valueint;
                            //nor_tuner[i].tSatellite.e22KHZFlag = cJSON_GetObjectItem(this,"22K")->valueint;
                            //nor_tuner[i].tSatellite.ePortNumber = cJSON_GetObjectItem(this,"Diseqc")->valueint;
                            break;

                        case eDVBCORE_SIGNAL_SOURCE_DVBC:
                            nor_tuner[i].tCable.u8TunerId = cJSON_GetObjectItem(this, "t_id")->valueint;
                            nor_tuner[i].tCable.u32Freq = cJSON_GetObjectItem(this, "Fre")->valueint;
                            nor_tuner[i].tCable.u32SymbRate = cJSON_GetObjectItem(this, "Sym")->valueint;
                            nor_tuner[i].tCable.eMod = cJSON_GetObjectItem(this, "Mod")->valueint;
                            break;

                        case eDVBCORE_SIGNAL_SOURCE_DVBT:
                        case eDVBCORE_SIGNAL_SOURCE_DMBT:
                            break;

                        case eDVBCORE_SIGNAL_SOURCE_ANALOG:
                            break;


                        }
                    }

                }
                else
                {
                    tCfg.u32NormalTpNum = 0;
                    tCfg.puNormalTpDescList = NULL;
                }
            }
            tCfg.u32NormalTpNum = count;
            tCfg.puNormalTpDescList = nor_tuner;
        }


    }

    cJSON_Delete(root);
    Installation_Start(tCfg);


    return 87;
}



int NpInstallation_Stop(void)
{
	installation_debug(__FUNCTION__);
    Installation_Stop();
    return 0;
}


int NpInstallation_Get_SearchRet(void)
{
	installation_debug(__FUNCTION__);
    return 0;
}

int NpInstallation_Free_SearchRet(void)
{
	installation_debug(__FUNCTION__);
    Installation_Free_SearchRet();
    return 0;
}

int NpInstallation_Get_Tptotal(void)
{
	installation_debug(__FUNCTION__);
    return g_result_tptotal;
}

int NpInstallation_Get_Chtotal(void)
{
	installation_debug(__FUNCTION__);
    return g_result_chtotal;
}

void NpInstallation_Retstr_Free(void)
{
	installation_debug(__FUNCTION__);
    if(install_outstr != NULL)
    {
        free(install_outstr);
        install_outstr = NULL;
    }
}



/*

{	"search_type":	0,
	"to":{"Tu":1000,"NAct":10000,"NOth":10000,"BAT":10000,"PAT":5000,"PMT":	5000,"SAct":10000,"SOth":10000},
	"matpNum":1,"matp":[	{"Souce":2,"u":{"t_id":0,"Fre":698000,"Sym":6875,"Mod":7}}],
	"nortpNum":2,"nortp":[	{"Souce":2,"u":{"t_id":	0,"Fre":698000,"Sym":6875,"Mod":7}},
				{"Souce":2,"u":{"t_id":	0,"Fre":714000,"Sym":6875,"Mod":7}}]
}




void NpInstallation_test_create(void)
{
	int i=0;
	Install_Cfg_t tCfg;
	Install_Timeout_t tTimeout;
	DVBCore_Tuner_Desc_u main_tuner[2];
	DVBCore_Tuner_Desc_u nor_tuner[2];

	cJSON *root,*tp,*fir,*sec,*thi;
	char *out;
	int strLen;


    memset(&tCfg, 0, sizeof(Install_Cfg_t));
    memset(&tTimeout, 0, sizeof(Install_Timeout_t));
	memset(main_tuner, 0, 2*sizeof(DVBCore_Tuner_Desc_u));
	memset(nor_tuner, 0, 2*sizeof(DVBCore_Tuner_Desc_u));

	main_tuner[0].tCable.eSignalSource = eDVBCORE_SIGNAL_SOURCE_DVBC;
    main_tuner[0].tCable.u32Freq = 698000;
    main_tuner[0].tCable.u32SymbRate = 6875;
    main_tuner[0].tCable.eMod = (DVBCore_Modulation_e)7;
    main_tuner[0].tCable.u8TunerId = 0;

	main_tuner[1].tCable.eSignalSource = eDVBCORE_SIGNAL_SOURCE_DVBC;
    main_tuner[1].tCable.u32Freq = 706000;
    main_tuner[1].tCable.u32SymbRate = 6875;
    main_tuner[1].tCable.eMod = (DVBCore_Modulation_e)7;
    main_tuner[1].tCable.u8TunerId = 0;

	nor_tuner[0].tCable.eSignalSource = eDVBCORE_SIGNAL_SOURCE_DVBC;
    nor_tuner[0].tCable.u32Freq = 698000;
    nor_tuner[0].tCable.u32SymbRate = 6875;
    nor_tuner[0].tCable.eMod = (DVBCore_Modulation_e)7;
    nor_tuner[0].tCable.u8TunerId = 0;

	nor_tuner[1].tCable.eSignalSource = eDVBCORE_SIGNAL_SOURCE_DVBC;
    nor_tuner[1].tCable.u32Freq = 714000;
    nor_tuner[1].tCable.u32SymbRate = 6875;
    nor_tuner[1].tCable.eMod = (DVBCore_Modulation_e)7;
    nor_tuner[1].tCable.u8TunerId = 0;



	tTimeout.u32TunerTimeout = 1000;
    tTimeout.u32NITActualTimeout = 10000;
    tTimeout.u32NITOtherTimeout = 10000;
    tTimeout.u32BATTimeout = 10000;
    tTimeout.u32PATTimeout = 5000;
    tTimeout.u32PMTTimeout = 5000;
    tTimeout.u32SDTActualTimeout = 10000;
    tTimeout.u32SDTOtherTimeout = 10000;

	tCfg.eSearchType = eAUTO_SEARCH;
	tCfg.ptTimeout = &tTimeout;
	tCfg.u32MainTpNum = 0;
	tCfg.puMainTpDescList = main_tuner;

	tCfg.u32NormalTpNum = 2;
	tCfg.puNormalTpDescList = nor_tuner;


	root=cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "search_type",tCfg.eSearchType);


#if 1
	cJSON_AddItemToObject(root, "to", fir=cJSON_CreateObject());
		cJSON_AddNumberToObject(fir, "Tu",tTimeout.u32TunerTimeout);
		cJSON_AddNumberToObject(fir, "NAct",tTimeout.u32NITActualTimeout);
		cJSON_AddNumberToObject(fir, "NOth",tTimeout.u32NITOtherTimeout);
		cJSON_AddNumberToObject(fir, "BAT",tTimeout.u32BATTimeout);
		cJSON_AddNumberToObject(fir, "PAT",tTimeout.u32PATTimeout);
		cJSON_AddNumberToObject(fir, "PMT",tTimeout.u32PMTTimeout);
		cJSON_AddNumberToObject(fir, "SAct",tTimeout.u32SDTActualTimeout);
		cJSON_AddNumberToObject(fir, "SOth",tTimeout.u32SDTOtherTimeout);
#endif


#if 1

	cJSON_AddNumberToObject(root, "matpNum",2);


	cJSON_AddItemToObject(root,"matp", tp=cJSON_CreateArray());
	for (i=0;i<2;i++)
	{
		cJSON_AddItemToObject(tp, "ts", sec=cJSON_CreateObject());
		cJSON_AddNumberToObject(sec, "Souce", main_tuner[i].tCable.eSignalSource);
		cJSON_AddItemToObject(sec, "u", thi=cJSON_CreateObject());

		switch(main_tuner[i].tCable.eSignalSource)
		{
			case eDVBCORE_SIGNAL_SOURCE_DVBS:
			case eDVBCORE_SIGNAL_SOURCE_DVBS2:
				break;

			case eDVBCORE_SIGNAL_SOURCE_DVBC:
				cJSON_AddNumberToObject(thi, "t_id",main_tuner[i].tCable.u8TunerId);
				cJSON_AddNumberToObject(thi, "Fre", main_tuner[i].tCable.u32Freq);
				cJSON_AddNumberToObject(thi, "Sym", main_tuner[i].tCable.u32SymbRate);
				cJSON_AddNumberToObject(thi, "Mod", main_tuner[i].tCable.eMod);
				break;

			case eDVBCORE_SIGNAL_SOURCE_DVBT:
			case eDVBCORE_SIGNAL_SOURCE_DMBT:
				break;

			case eDVBCORE_SIGNAL_SOURCE_ANALOG:
				break;


		}
	}
#endif


	cJSON_AddNumberToObject(root, "nortpNum",2);
#if 1
	cJSON_AddItemToObject(root,"nortp", tp=cJSON_CreateArray());
	for (i=0;i<2;i++)
	{
		cJSON_AddItemToObject(tp, "ts", sec=cJSON_CreateObject());
		cJSON_AddNumberToObject(sec, "Souce", nor_tuner[i].tCable.eSignalSource);
		cJSON_AddItemToObject(sec, "u", thi=cJSON_CreateObject());

		switch(nor_tuner[i].tCable.eSignalSource)
		{
			case eDVBCORE_SIGNAL_SOURCE_DVBS:
			case eDVBCORE_SIGNAL_SOURCE_DVBS2:
				break;

			case eDVBCORE_SIGNAL_SOURCE_DVBC:
				cJSON_AddNumberToObject(thi, "t_id",nor_tuner[i].tCable.u8TunerId);
				cJSON_AddNumberToObject(thi, "Fre", nor_tuner[i].tCable.u32Freq);
				cJSON_AddNumberToObject(thi, "Sym", nor_tuner[i].tCable.u32SymbRate);
				cJSON_AddNumberToObject(thi, "Mod", nor_tuner[i].tCable.eMod);
				break;

			case eDVBCORE_SIGNAL_SOURCE_DVBT:
			case eDVBCORE_SIGNAL_SOURCE_DMBT:
				break;

			case eDVBCORE_SIGNAL_SOURCE_ANALOG:
				break;


		}
	}
#endif

	out=cJSON_Print(root);
	cJSON_Delete(root);

	if(install_outstr!=NULL)
	{
		free(install_outstr);
		install_outstr = NULL;
	}

	strLen = strlen(out);
	if(strLen>0)
	{
		install_outstr = (char *)malloc(strLen + 10);
		if(install_outstr == NULL)
		{
			return install_outstr;
		}
		sprintf( install_outstr, "%s", out);
	#if 0
		{
			FILE* fp = NULL;

			fp = fopen("/mnt/sda/sda1/1.txt","wr+");
			if(fp !=NULL)
			{
				fwrite(install_outstr, strLen, 1, fp);
				fclose(fp);
			}
		}
		#endif
	}

	free(out);




}
*/
#endif


