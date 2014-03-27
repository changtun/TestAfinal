/******************************************************************************

                  ??????? (C), 2001-2011, ??????????????????????

 ******************************************************************************
  ?? ?? ??   : file_config.c
  ?? ?? ??   : ????
  ??    ??   : zxguan
  ???????   : 2013??4??15??
  ??????   :
  ????????   : Config FIle
  ?????§Ò?   :
  ??????   :
  1.??    ??   : 2013??4??15??
    ??    ??   : zxguan
    ???????   : ???????

******************************************************************************/

/*----------------------------------------------*
 * ??????                                   *
 *----------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "osapi.h"
#include "pbitrace.h"
#include "file_config.h"
/*----------------------------------------------*
 * ?????????                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ????????????                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ?????????????                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ??????                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ??ýD????                                   *
 *----------------------------------------------*/

#define MAX_EXPLAN_NUMBER 3
#define MAX_FILE_BUFFER  100
#define MAX_EXPLAN_BUFFER 512

typedef struct
{
    eTUNER_BAND_WIDTH eTunerBandWidth;
    eSEARCH_BAT_SWITCH eSearchBatSwitch;
    eDEFAULT_CHARACTER_ENCODE eDefaultCharacterEncode;
    U8 *pExplan[MAX_EXPLAN_NUMBER];
}FILE_CONFIG_m;

/*----------------------------------------------*
 * ????????                                     *
 *----------------------------------------------*/
FILE_CONFIG_m m_FileConfig;
/*----------------------------------------------*
 * ????                                       *
 *----------------------------------------------*/
#define CONFIG_FILE_PATH "/mnt/expand/dvb_config"

#define CONFIG_NAME_TUNER_BAND_WIDTH            "TunerBandWidth"
#define CONFIG_NAME_SEARCH_BAT_SWITCH           "SearchBAT"
#define CONFIG_NAME_DEFAULT_CHARACTER_ENCODE    "DefaultCharacterEncode"

static int _GetMinLength(char *pNameA, char *pNameB )
{
   int lenA = 0;
   int lenB = 0;
   lenA = strlen(pNameA);
   lenB = strlen(pNameB);
   return ((lenA > lenB)? lenA : lenB);//MIN_LENGTH(pNameA,pNameB);
}

static int _InsertConfig( char *pName, char *pValue, char *pExplan )
{
    int iRet = 0;
    int iValue = 0;
    char *pTemp = NULL;
    
    iRet = strncasecmp( pName, CONFIG_NAME_TUNER_BAND_WIDTH, _GetMinLength(pName,CONFIG_NAME_TUNER_BAND_WIDTH) );
    if( 0 == iRet )
    {
        sscanf( pValue,"%d", &iValue);
        m_FileConfig.eTunerBandWidth = iValue;
        m_FileConfig.pExplan[0] = pExplan;
        return 0;
    }

    iRet = strncasecmp( pName, CONFIG_NAME_SEARCH_BAT_SWITCH, _GetMinLength(pName,CONFIG_NAME_SEARCH_BAT_SWITCH) );
    if( 0 == iRet )
    {
        sscanf( pValue,"%d", &iValue);
        m_FileConfig.eSearchBatSwitch = iValue;
        m_FileConfig.pExplan[1] = pExplan;
        return 0;
    }

    iRet = strncasecmp( pName, CONFIG_NAME_DEFAULT_CHARACTER_ENCODE, _GetMinLength(pName,CONFIG_NAME_DEFAULT_CHARACTER_ENCODE) );
    if( 0 == iRet )
    {
        sscanf( pValue,"%d", &iValue);
        m_FileConfig.eDefaultCharacterEncode = iValue;
        m_FileConfig.pExplan[2] = pExplan;
        return 0;
    }
    
    return 0;
}

static void _ReadConfig()
{
    int iRet = 0;
    FILE *pFp = NULL;
    char cBuffer[MAX_FILE_BUFFER];
    char cName[50];
    char cValue[50];
    char *pTemp = NULL;
    char *pExTemp = NULL;
    char cExplan[MAX_EXPLAN_BUFFER];
    
    iRet = access(CONFIG_FILE_PATH, F_OK );
    if( 0 != iRet )
    {
        pbierror("[%s %d] DVB Config File Not Find Error.\n",DEBUG_LOG);
        return;
    }

	iRet = access(CONFIG_FILE_PATH, W_OK | R_OK	);
    if( 0 != iRet )
    {
        pbierror("[%s %d] DVB Config File Can't Read Or Write.\n",DEBUG_LOG);
#define CHMODCMD "chmod 777 "CONFIG_FILE_PATH
        pbiinfo("[%s %d] Commnd [%s].\n",DEBUG_LOG, CHMODCMD);
		system(CHMODCMD);
#undef CHMODCMD
    }

    pFp = fopen( CONFIG_FILE_PATH, "r");
    if( NULL == pFp )
    {
        pbierror("[%s %d] DVB Config Open Error.\n",DEBUG_LOG);
        return;
    }

    memset( cBuffer, 0, MAX_FILE_BUFFER );
    memset( cExplan, 0, MAX_EXPLAN_BUFFER );
    while( fgets( cBuffer, MAX_FILE_BUFFER, pFp) )
    {
        if( '#' == cBuffer[0] )
        {
            sprintf( cExplan, "%s%s", cExplan, cBuffer );
            memset( cBuffer, 0, MAX_FILE_BUFFER );
            continue;
        }

        pTemp = strchr( cBuffer, '\r');
        if( NULL != pTemp )
        {
            *pTemp = '\0';
        }

        pTemp = strrchr( cBuffer, '.');
        if( NULL != pTemp )
        {
            *pTemp = '\0';
        }
        
        pTemp = strchr( cBuffer, '=');
        if( NULL == pTemp )
        {
            memset( cBuffer, 0, MAX_FILE_BUFFER );
            continue;
        }
        *pTemp = '\0';
        pTemp++;
        
        memset( cName, 0, 50);
        memset( cValue, 0, 50);
        
        memcpy( cName, cBuffer, strlen(cBuffer));
        memcpy( cValue, pTemp, strlen(pTemp) );

        pExTemp = OS_MemAllocate(system_p, MAX_EXPLAN_BUFFER);
        if( NULL != pExTemp )
        {   
            memcpy( pExTemp, cExplan, MAX_EXPLAN_BUFFER );
            pExTemp[MAX_EXPLAN_BUFFER] = '\0';
        }
        
        _InsertConfig( cName, cValue, pExTemp );
        memset( cExplan, 0, MAX_EXPLAN_BUFFER );
        memset( cBuffer, 0, MAX_FILE_BUFFER );
        pExTemp = NULL;
    }

    fclose(pFp);
}

static void _WriteConfig()
{
    int op = 0;
    int sRet = 0;
    char cBuffer[256] = {0};

    if( 0 == access(CONFIG_FILE_PATH, F_OK))
    {
#define CHMODCMD "chmod 777 "CONFIG_FILE_PATH
        pbiinfo("[%s %d] Commnd [%s].\n",DEBUG_LOG, CHMODCMD);
		system(CHMODCMD);
#undef CHMODCMD
    }
    
    op = open( CONFIG_FILE_PATH, O_CREAT|O_SYNC|O_TRUNC|O_WRONLY,644);
    if( 0 == op )
    {
        return -1;
    }

    if( NULL != m_FileConfig.pExplan[0] )
    {
        sRet = write( op, m_FileConfig.pExplan[0], strlen(m_FileConfig.pExplan[0]));
    }
    memset(cBuffer,0,256);
    sprintf(cBuffer,"%s=%d.\n", CONFIG_NAME_TUNER_BAND_WIDTH, m_FileConfig.eTunerBandWidth);
    sRet = write( op, cBuffer, strlen(cBuffer));

    if( NULL != m_FileConfig.pExplan[1] )
    {
        sRet = write( op, m_FileConfig.pExplan[1], strlen(m_FileConfig.pExplan[1]));
    }
    memset(cBuffer,0,256);
    sprintf(cBuffer,"%s=%d.\n", CONFIG_NAME_SEARCH_BAT_SWITCH, m_FileConfig.eSearchBatSwitch);
    sRet = write( op, cBuffer, strlen(cBuffer));

    if( NULL != m_FileConfig.pExplan[2] )
    {
        sRet = write( op, m_FileConfig.pExplan[2], strlen(m_FileConfig.pExplan[2]));
    }
    memset(cBuffer,0,256);
    sprintf(cBuffer,"%s=%d.\n", CONFIG_NAME_DEFAULT_CHARACTER_ENCODE, m_FileConfig.eDefaultCharacterEncode);
    sRet = write( op, cBuffer, strlen(cBuffer));

    close(op);

}

void _FreeExplan()
{
    int ii = 0;
    for(; ii < MAX_EXPLAN_NUMBER; ii++ )
    {
        if( NULL != m_FileConfig.pExplan[ii] )
        {
            OS_MemDeallocate(system_p, m_FileConfig.pExplan[ii] );
            m_FileConfig.pExplan[ii] = NULL;
        }
    }
}

void _ShowConfig()
{
    pbiinfo("\n====================Show Config======================================\n");
    if( NULL != m_FileConfig.pExplan[0] )
    {
        pbiinfo("%s",m_FileConfig.pExplan[0]);
    }
    pbiinfo("TunerBandWidth = %d.\n",m_FileConfig.eTunerBandWidth);
    if( NULL != m_FileConfig.pExplan[1] )
    {
        pbiinfo("%s",m_FileConfig.pExplan[1]);
    }
    pbiinfo("SearchBatSwitch = %d.\n",m_FileConfig.eSearchBatSwitch);
    if( NULL != m_FileConfig.pExplan[2] )
    {
        pbiinfo("%s",m_FileConfig.pExplan[2]);
    }
    pbiinfo("DefaultCharacterEncode = %d.\n",\
        m_FileConfig.eDefaultCharacterEncode);
    pbiinfo("========================Show End=====================================\n\n");
}

void ConfigInit()
{
  // pbiinfo("\n[%s %d] DVB Config Init.\n",DEBUG_LOG);
   m_FileConfig.eTunerBandWidth = BAND_WIDTH_6M;
   m_FileConfig.eSearchBatSwitch = SEARCH_BAT_OFF;
   m_FileConfig.eDefaultCharacterEncode = CHARACTER_BIG5;
   _ReadConfig();
   _ShowConfig();
   _FreeExplan();
  // pbiinfo("[%s %d] DVB Config Init End.\n",DEBUG_LOG);
}

void SetDVBConfig(CONFIG_WRITE_T tWriteInfo)
{
    _ReadConfig();
    SetTunerBandWidth( tWriteInfo.eBandWidth );
    SetSearchBatSwitch( tWriteInfo.eBatSwitch );
    SetDefaultCharacterEncode( tWriteInfo.eEncode );
    _WriteConfig();
    _ShowConfig();
    _FreeExplan();
}

void GetDVBConfig(CONFIG_WRITE_T *ptInfo)
{
	if( NULL == ptInfo)
	{
		return;
	}
	_ReadConfig();
	ptInfo->eBandWidth = GetTunerBandWidth();
	ptInfo->eBatSwitch = GetSearchBatSwitch();
	ptInfo->eEncode = GetDefaultCharacterEncode();
}

eTUNER_BAND_WIDTH GetTunerBandWidth()
{
    return m_FileConfig.eTunerBandWidth;
}

void SetTunerBandWidth(eTUNER_BAND_WIDTH eBandWidth)
{
    m_FileConfig.eTunerBandWidth = eBandWidth;
}

eSEARCH_BAT_SWITCH GetSearchBatSwitch()
{
    return m_FileConfig.eSearchBatSwitch;
}

void SetSearchBatSwitch(eSEARCH_BAT_SWITCH eBatSwitch)
{
    m_FileConfig.eSearchBatSwitch = eBatSwitch;
}

eDEFAULT_CHARACTER_ENCODE GetDefaultCharacterEncode()
{
    return m_FileConfig.eDefaultCharacterEncode;
}

void SetDefaultCharacterEncode(eDEFAULT_CHARACTER_ENCODE eEncode)
{
    m_FileConfig.eDefaultCharacterEncode = eEncode;
}
