/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name : dal_hls.c
* Description : 
* History : 
*	Date				Modification				Name
*	----------			------------			----------
*	2013/5/13			Created					nlv	
******************************************************************************/
/*-----------------------------------------------------*/
/*              Includes												*/
/*-----------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl.h>
#include <types.h>
#include <easy.h>
//#include <pthread.h>
#include "pbitrace.h"

#include "pvlist.h"
#include "osapi.h"
#include "pvddefs.h"
#include "ring_buffer.h"
#include "dal_common.h"
#include "dal_wrap.h"
#include "dal_message.h"
#include "dal_download.h"
#include "dal_hls.h"
#include "dal.h"
#include "dal_core.h"
#include "anpdebug.h"


/*-----------------------------------------------------*/
/*              Global												*/
/*-----------------------------------------------------*/

static M3U8_struct_t 		g_M3u8_Info;
static DAL_M3u8Control_t	g_M3u8Control;

//C8 HLS_HeadName[256]="/data/data/com.pbi.dvb/m3u8header.txt";
//C8 HLS_FileName[256]="/data/data/com.pbi.dvb/m3u8context.txt";
C8 HLS_HeadName[256];
C8 HLS_FileName[256];

static U32				   num = 0;
static U8				trynum=0;
static U32 				hls_movie_playtime = 0;
static U8				HLS_resume_flag=0;


extern DAL_DownloadControl_t	g_DownloadControl;
extern C8 hls_http[256];




/*-----------------------------------------------------*/
/*              Exported Types									*/
/*-----------------------------------------------------*/

static S32 add_one_hls(Current_hls_t hlsnode);
static S32 check_hls_exit(Current_hls_t hlsnode);
static void* dal_m3u8_request_task(void* param);
static void get_real_httpaddress( DAL_Message_t* pMsg);
static void parse_firstlevel_m3u8(DAL_Message_t* pMsg);
static void parse_secondlevel_m3u8( DAL_Message_t* pMsg);
static void ts_download_start(DAL_Message_t* pMsg);
static void stop_request_m3u8( DAL_Message_t*  pMsg);
static void del_all_hls(void);
static void m3u8_download_queue_clear( void);

/*-----------------------------------------------------*/
/*              Local Function Prototypes					*/
/*-----------------------------------------------------*/
static S32 get_http_Respon(C8* string,U32* answer)
{
	C8*pstr=NULL;
	
	pstr=strchr(string, '.');
	if(pstr!=NULL)
	{
		pstr+=3;
		*answer = atoi(pstr);
	}
	DebugMessage("[get_http_Respon]:%d %s!",*answer,pstr);
	return -1;

}

/*------------------------------------------------------------------*/
/* 0: http header 获取成功，但是没有location 地址			*/
/* 1: http header 获取成功，收到location地址  				*/
/* -1:没有需要的字符地址									*/
/* -2:parse 数据失败												*/
/*------------------------------------------------------------------*/
static S32 get_http_Address(C8* string,C8* UseAddr)
{
	C8*pstr=NULL;
	U32 len=0;
	
	pstr=strchr(string, ':');
	if(pstr!=NULL)
	{
		pstr+=2;
		
	//	DebugMessage("[get_http_Address]: strlen:%d! -%d-",strlen(pstr),pstr[strlen(pstr)-1]);
		len=strlen(pstr);
		if(pstr[len-1]== 13)
			len-=1;
		if(strncpy(UseAddr,pstr,len)!=NULL)/*去掉回车*/
		{
			return 0;/*OK*/
		}
		else
		{
			DebugMessage("[get_http_Address]: strcpy error!");
			return -1;
		}
	}
	DebugMessage("[get_http_Address]: no we need char!");
	return -1;

}
static S32 is_file_empty(void *filehandle)
{
	U32 ulength = 0;
	FILE *fp = (FILE *)filehandle;
	
	fseek(fp, 0, SEEK_END); 
	ulength = ftell(fp);	
	fseek(fp, 0, SEEK_SET);

	return ulength;

}

/*------------------------------------------------------------------*/
/* 0: http header 获取成功，但是没有location 地址			*/
/* 1: http header 获取成功，收到location地址  				*/
/* -1:文件打开失败											*/
/* -2:parse 数据失败												*/
/*------------------------------------------------------------------*/
static S32 dal_parse_first_http_header(C8* header_name,C8 *UseAddr)
{
	FILE* filefp = NULL;
	C8 cString[512];
	U32 num=0;

	DAL_SemWait(g_M3u8_Info.sem_request);

	filefp = fopen(header_name,"r");
	DebugMessage("[dal_parse_first_http_header]: open file headle%x!",filefp);
	if(filefp == NULL)
	{
		DebugMessage("[dal_parse_first_http_header]: open file error!");
		
		DAL_SemSignal(g_M3u8_Info.sem_request);
		return -1;
	}
	
	if(is_file_empty(filefp) ==0 )
	{
		DebugMessage("[dal_parse_first_http_header]: empty http header !");
		fclose(filefp);
		
		DAL_SemSignal(g_M3u8_Info.sem_request);
		DebugMessage("[dal_parse_first_http_header]: close file headle%x!-%d",filefp,__LINE__);
		return -2;
	}

	fseek(filefp,0,SEEK_SET);
	while ( !feof(filefp) )
	{
		memset(cString, 0, sizeof(cString));
		fscanf(filefp,"%[^\n]%*c",cString);
		if(!strncmp(cString,HTTP_HEADER_TAG,7))
		{
			get_http_Respon(cString,&num);
			if((num != 302)&&(num != 200))
			{
				fclose(filefp);
				
				DAL_SemSignal(g_M3u8_Info.sem_request);
				DebugMessage("[dal_parse_first_http_header]: close file headle%x!-%d",filefp,__LINE__);
				return -2;
			}
		}
		if(!strncmp(cString,URL_TAG,9))
		{
			/*找到下级http请求地址，取出http请求地址*/
			if(get_http_Address(cString,UseAddr)!= 0)
			{
				DebugMessage("[dal_parse_first_http_header]: find http address error!");	
				fclose(filefp);
				
				DAL_SemSignal(g_M3u8_Info.sem_request);
				DebugMessage("[dal_parse_first_http_header]: close file headle%x!-%d",filefp,__LINE__);
				return -2;
			}
			else
			{
				DebugMessage("[dal_parse_first_http_header]: find http address success!");/*OK!*/	
				fclose(filefp);
				
				DAL_SemSignal(g_M3u8_Info.sem_request);
				DebugMessage("[dal_parse_first_http_header]: close file headle%x!-%d",filefp,__LINE__);
				return 1;
			}
		}
		
	}
	
	/*未找到下级http 请求地址，考虑是否已经请求到了m3u8文件*/
	DebugMessage("[dal_parse_first_http_header]: no http address !");
	fclose(filefp);
	
	DebugMessage("[dal_parse_first_http_header]: close file headle%x!-%d",filefp,__LINE__);
	
	DAL_SemSignal(g_M3u8_Info.sem_request);
	return 0;
}

static U32 Get_m3u8_fileElement(const C8* CString)
{
/*m3u8*/	
	if(!strncmp(CString,M3U8_TAG, 7))				/*#EXTM3U*/
		return eM3U8_ELEMENT_TAG;
	if(!strncmp(CString,M3U8_CATCHE, 19))			/*#EXT-X-ALLOW-CACHE:*/
		return eM3U8_ELEMENT_CATCHE;
	if(!strncmp(CString,M3U8_TARGETDURATION, 22))	/*#EXT-X-TARGETDURATION:*/
		return eM3U8_ELEMENT_TARGETDURATION;
	if(!strncmp(CString,M3U8_SEQUENCE, 22))		/*#EXT-X-MEDIA-SEQUENCE:*/	
		return eM3U8_ELEMENT_SEQUENCE;
	if(!strncmp(CString,M3U8_STREAMINFO,18))		/*#EXT-X-STREAM-INF:*/	
		return eM3U8_ELEMENT_STREAMINFO;	
	if(!strncmp(CString,M3U8_KEY, 11))				/*#EXT-X-KEY:*/	
		return eM3U8_ELEMENT_KEY;
	if(!strncmp(CString,M3U8_DISCONTINUITY, 20))	/*#EXT-X-DISCONTINUITY*/	
		return eM3U8_ELEMENT_DISCONTINUITY;
	if(!strncmp(CString,M3U8_EXTINF, 8))			/*#EXTINF:*/
		return eM3U8_ELEMENT_EXTINF;
	if(!strncmp(CString,M3U8_ENDLIST, 14))			/*#EXT-X-ENDLIST*/
		return eM3U8_ELEMENT_ENDLIST;
	if(!strncmp(CString,HTTP_URL, 5))			/*http*/
		return eM3U8_ELEMENT_HTTP;

	return eM3U8_ELEMENT_UNKNOW;

}	


/*------------------------------------------------------------------*/
/* 1: 不是顶级m3u8 ,按照2级文件解析						*/
/* 0: 解析完成顶级m3u8 文件								*/
/* -1: 解析失败													*/
/*------------------------------------------------------------------*/
static S32 dal_parse_first_level_m3u8(C8* file_name)
{
	FILE* filefp = NULL;
	C8 cString[512];
	U8 find_m3u8=0,i=0;
	
	DAL_SemWait(g_M3u8_Info.sem_request);
	filefp = fopen(file_name,"r");
	
	DebugMessage("[dal_parse_first_level_m3u8]: open file headle%x!",filefp);
	if(filefp == NULL)
	{
	
		DAL_SemSignal(g_M3u8_Info.sem_request);
		DebugMessage("[dal_parse_first_level_m3u8]: open file error!");
		return -1;
	}
	
	if(is_file_empty(filefp) ==0 )
	{
		DebugMessage("[dal_parse_first_level_m3u8]: empty m3u8 file !");
		fclose(filefp);
		
		DAL_SemSignal(g_M3u8_Info.sem_request);
		DebugMessage("[dal_parse_first_level_m3u8]: close file headle%x!-%d",filefp,__LINE__);
		return -1;
	}

	fseek(filefp,0,SEEK_SET);
	
	memset(cString, 0, sizeof(cString));
	fscanf(filefp,"%[^\n]%*c",cString);
	
	if (strncmp(cString, M3U8_TAG, 7) != 0)
	{
		DebugMessage("[dal_parse_first_level_m3u8] missing #EXTM3U tag .. aborting");
		fclose(filefp);
		
		DAL_SemSignal(g_M3u8_Info.sem_request);
		DebugMessage("[dal_parse_first_level_m3u8]: close file headle%x!-%d",filefp,__LINE__);
		return -1;
	}
	while ( !feof(filefp) )
	{
		memset(cString, 0, sizeof(cString));
		//i=fscanf(filefp,"%[^\n]%*c",cString);
		i=fscanf(filefp,"%s",cString);
		
		if(i==0)
		{
			DebugMessage("[dal_parse_first_level_m3u8]: kong hang hang");
			continue;
		}
		switch(Get_m3u8_fileElement(cString))
		{
			
			case eM3U8_ELEMENT_STREAMINFO:/*有顶级m3u8文件*/
			{
				/*暂时没做顶级解析*/
				find_m3u8=1;
				
				DebugMessage("[dal_parse_first_level_m3u8] find first level m3u8，%s",cString);
			}	
			break;	
		}
		if(find_m3u8==1)
		{
			DebugMessage("[dal_parse_first_level_m3u8] find first level m3u8，but we donot parse!!!");
			fclose(filefp);
			
			DAL_SemSignal(g_M3u8_Info.sem_request);
			DebugMessage("[dal_parse_first_level_m3u8]: close file headle%x!-%d",filefp,__LINE__);
			return 0;
		}
		
	}
	
	DebugMessage("[dal_parse_first_level_m3u8] no first level m3u8,parse second level m3u8!!!");
	fclose(filefp);
	
	DebugMessage("[dal_parse_first_level_m3u8]: close file headle%x!-%d",filefp,__LINE__);
	
	DAL_SemSignal(g_M3u8_Info.sem_request);
	return 1;
}

/*------------------------------------------------------------------*/
/* 0: 获取成功								*/
/* -1: 获取失败												*/
/*------------------------------------------------------------------*/
static S32 get_cache_state(C8* string,U8 *cache)
{
	C8*pstr=NULL;
	
	pstr = strchr(string, ':');
	if(pstr!=NULL)
	{
		pstr++;
		
		DebugMessage("[%s]",pstr);
		if((strncmp(pstr,"YES",3)==0)||(strncmp(pstr,"yes",3)== 0))
			*cache = 1;
		else
			*cache = 0;
	}
	else
	{
		DebugMessage("[get_cache_state]: point NULL!");
		return -1;
	}
	
	DebugMessage("[get_cache_state]: cache[%d] ",*cache);
	return 0;

}

/*------------------------------------------------------------------*/
/* 0: 获取成功								*/
/* -1: 获取失败												*/
/*------------------------------------------------------------------*/
static S32 get_duration_time(C8* string,U32 *target_time)
{
	C8*pstr=NULL;
	
	pstr = strchr(string, ':');
	if(pstr!=NULL)
	{
		pstr++;
		*target_time=atoi(pstr);
	}
	else
	{
		DebugMessage("[get_duration_time]: point NULL!");
		return -1;
	}
	return 0;

}

/*------------------------------------------------------------------*/
/* 0: 获取成功								*/
/* -1: 获取失败												*/
/*------------------------------------------------------------------*/
static S32 get_sequence_num(C8* string,U32 *sequence_num)
{
	C8*pstr=NULL;
	
	pstr = strchr(string, ':');
	if(pstr!=NULL)
	{
		pstr++;
		*sequence_num=atoi(pstr);
	}
	else
	{
		DebugMessage("[get_sequence_num]: point NULL!");
		return -1;
	}
	return 0;

}
/*------------------------------------------------------------------*/
/* 0: 获取成功								*/
/* -1: 获取失败												*/
/*------------------------------------------------------------------*/
static S32 get_key_num(C8* string,U32 *key_num)
{
	C8*pstr=NULL;
	
	pstr = strchr(string, ':');
	if(pstr!=NULL)
	{
		pstr++;
		*key_num=atoi(pstr);
	}
	else
	{
		DebugMessage("[get_key_num]: point NULL!");
		return -1;
	}
	return 0;

}

/*------------------------------------------------------------------*/
/* 0: 获取成功								*/
/* -1: 获取失败												*/
/*------------------------------------------------------------------*/
static S32 get_inf_num(C8* string,U32 *inf_num)
{
	C8* pstr=NULL;
	C8 str[256];
	U32 uTotallength=0;
	
	uTotallength=strlen(string);
	memset(str,0,sizeof(str));
	pstr = strchr(string, ':');
	if(pstr!=NULL)
	{
		pstr++;
		strncpy(str, pstr,uTotallength-9);
		*inf_num=atoi(str);	
	}
	else
	{
		DebugMessage("[get_inf_num]: point NULL!");
		return -1;
	}
	return 0;
}

/*------------------------------------------------------------------*/
/* 0: 解析完成													*/
/* -1: 解析失败													*/
/*------------------------------------------------------------------*/
static S32 dal_parse_second_level_m3u8(C8* file_name)
{
	FILE* filefp = NULL;
	C8 cString[256];
	U8 find=0;
	
	DAL_SemWait(g_M3u8_Info.sem_request);
	filefp = fopen(file_name,"r");
	
	DebugMessage("[dal_parse_second_level_m3u8]: open file headle%x!",filefp);
	if(filefp == NULL)
	{
	
		DAL_SemSignal(g_M3u8_Info.sem_request);
		DebugMessage("[dal_parse_second_level_m3u8]: open file error!");
		return -1;
	}
	
	DebugMessage("[dal_parse_second_level_m3u8]: 111111111111111111111111 !");
	if(is_file_empty(filefp) ==0 )
	{
		DebugMessage("[dal_parse_second_level_m3u8]: empty m3u8 file !");
		fclose(filefp);
		
		DAL_SemSignal(g_M3u8_Info.sem_request);
		DebugMessage("[dal_parse_second_level_m3u8]: close file headle%x!-%d",filefp,__LINE__);
		return -1;
	}

	fseek(filefp,0,SEEK_SET);
	
	memset(cString, 0, sizeof(cString));
	fscanf(filefp,"%[^\n]%*c",cString);
	
	if (strncmp(cString, M3U8_TAG, 7) != 0)
	{
		DebugMessage("[dal_parse_second_level_m3u8] missing #EXTM3U tag .. aborting");
		fclose(filefp);
		
		DAL_SemSignal(g_M3u8_Info.sem_request);
		DebugMessage("[dal_parse_second_level_m3u8]: close file headle%x!-%d",filefp,__LINE__);
		return -1;
	}
	
	DebugMessage("[dal_parse_second_level_m3u8]: 22222222222222222222 !");
	while ( !feof(filefp) )
	{
	
		memset(cString, 0, sizeof(cString));
		//fscanf(filefp,"%[^\n]%*c",cString);
		
		fscanf(filefp,"%s",cString);
		DebugMessage("[dal_parse_second_level_m3u8]: %s!",cString);
		switch(Get_m3u8_fileElement(cString))
		{
			case eM3U8_ELEMENT_CATCHE:
			{	
				get_cache_state(cString,&g_M3u8_Info.catche);
				DebugMessage("[dal_parse_second_level_m3u8]:33333333333[%d]3333333333",g_M3u8_Info.catche);
			}
			break;
			case eM3U8_ELEMENT_TARGETDURATION:
			{
				get_duration_time(cString,&g_M3u8_Info.duration);
				DebugMessage("[dal_parse_second_level_m3u8]:4444444444444444444444");
			}
			break;
			case eM3U8_ELEMENT_SEQUENCE:
			{
				get_sequence_num(cString,&g_M3u8_Info.sequence);
				
				DebugMessage("[dal_parse_second_level_m3u8]:55555555555555555555555");
			}
			break;
			case eM3U8_ELEMENT_KEY:
			{
				get_key_num(cString,&g_M3u8_Info.xkey);
				
				DebugMessage("[dal_parse_second_level_m3u8]:666666666666666666666");
			}
			break;	
			case eM3U8_ELEMENT_DISCONTINUITY:
			{
				g_M3u8_Info.discontinuity = 1;/*后段视频文件与前不连续*/
				
				DebugMessage("[dal_parse_second_level_m3u8]:777777777777777777777");
			}
			break;	
			case eM3U8_ELEMENT_EXTINF:
			{
				Current_hls_t  hls_info;
				
				memset(&hls_info,0,sizeof(Current_hls_t));
				get_inf_num(cString,&hls_info.segement_info);
				
				DebugMessage("[dal_parse_second_level_m3u8]:hls_info.segement_info:%d",hls_info.segement_info);
				
				memset(cString, 0, sizeof(cString));
				//fscanf(filefp,"%[^\n]%*c",cString);
				fscanf(filefp,"%s",cString);
				DebugMessage("[dal_parse_second_level_m3u8]:%s",cString);
				if (strncmp(cString, HTTP_URL, 5) != 0)//使用的相对地址
				{
					C8* str=NULL;		
					S32 length=0,Slen=0;
					C8 tmp[256];

					memset(tmp,0,sizeof(tmp));
					strcpy(tmp,hls_http);
					length=strlen(tmp);
					
					DebugMessage("[dal_parse_second_level_m3u8]:^^^^^%s^^^^%d",tmp,length);
					str=tmp+length-1;
					while(*str!='/')
					{
						str=str-1;
					}
					Slen=strlen(str);
					DebugMessage("[dal_parse_second_level_m3u8]:^^^^^%s^^^^%d",str,Slen);
						
					strncpy(hls_info.http_address,tmp,(length-Slen+1));
					
					strcat(hls_info.http_address,cString);
				}
				else//使用的绝对地址
				{
					strcpy(hls_info.http_address,cString);

					//	strcpy(hls_info.http_address,"http://192.168.30.251:7500/v1/tfs/L1Va_TByZT1RCvBVdK");
			
				}
				DebugMessage("[dal_parse_second_level_m3u8]:hls_info.http_address:%s",hls_info.http_address);

				if(g_M3u8_Info.catche == 1)
				{
					g_M3u8_Info.total_time = g_M3u8_Info.total_time + hls_info.segement_info;
					pbiinfo("[dal_parse_second_level_m3u8] total time [%d] !",g_M3u8_Info.total_time);
					if(add_one_hls(hls_info)!=0)	
						DebugMessage("[dal_parse_second_level_m3u8] add one hls failed 1 !");
				}
				else
				{
					if(check_hls_exit(hls_info)==0)
					{
						if((num!=0)&&(g_M3u8_Info.total_num==num))
						{
						
							//DAL_SemSignal(g_M3u8_Info.sem_request);
							DebugMessage("[dal_parse_second_level_m3u8] num =%d;g_M3u8_Info.total_num=%d!",num,g_M3u8_Info.total_num);
								num=0;
								del_all_hls();
						}
						if(add_one_hls(hls_info)!=0)	
							DebugMessage("[dal_parse_second_level_m3u8] add one hls failed 2 !");
					}
					else
					{
						find=1;
						
						DebugMessage("[dal_parse_second_level_m3u8] find the same address !");
						DAL_TaskDelay(3000);
					}
						
				}
				DebugMessage("[dal_parse_second_level_m3u8]:8888888888888888888888888");
			}
			break;	
			case eM3U8_ELEMENT_ENDLIST:
			{
				g_M3u8_Info.live_state= 1;/* no live*/
				
				DebugMessage("[dal_parse_second_level_m3u8]:9999999999999999");
			}
			break;
			default:
				DebugMessage("[dal_parse_second_level_m3u8] have not the element!!!");
			break;	
		}
		if( find==1)
			break;
	}
	
	DebugMessage("[dal_parse_second_level_m3u8]: 000000000000000000000 !");
	fclose(filefp);
	
	DebugMessage("[dal_parse_second_level_m3u8]: close file headle%x!-%d",filefp,__LINE__);
	DAL_SemSignal(g_M3u8_Info.sem_request);
	return 0;
}


static size_t  file_fwrite(char *buffer,
               size_t size,
               size_t nitems,
               void *userp)
{
	FILE *fp = (FILE *)userp;
	int iRet=0;
	
	if(fp == NULL)
		return -1;
	
	iRet =fwrite(buffer,size,nitems,fp);
	
	//DebugMessage("[file_fwrite] file_fwrit %c%c%c%c%c%c%c size:%d! fp:(%x)",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],size*nitems,fp);
	if ( 0 >= iRet )
	{
		DebugMessage("[file_fwrite] write error");
		return -1;
	}
	return nitems;
}

static S32 dal_m3u8_http_request(C8* URL,C8* header_name,C8* file_name)
{
		CURL* curl;
		CURLcode res;
		FILE* filefp = NULL;
		FILE* headerfp = NULL;

		DAL_SemWait(g_M3u8_Info.sem_request);

		curl_global_init(CURL_GLOBAL_DEFAULT);

		curl = curl_easy_init();
		if(curl == NULL)
		{
		
		DAL_SemSignal(g_M3u8_Info.sem_request);
			DebugMessage("[dal_m3u8_http_request]curl init failed\n");
			return -1;
		}

		/*open file which we want to get*/
		filefp = fopen(file_name,"w+");
		
		DebugMessage("[dal_m3u8_http_request]: open m3u8 file headle%x!",filefp);
		if(filefp == NULL)
		{
			DebugMessage("[dal_m3u8_http_request]:open get file error\n");
			curl_easy_cleanup(curl);
			
			DAL_SemSignal(g_M3u8_Info.sem_request);
			return -1;
		}

		/*open a temp file which store the http header */	
		headerfp = fopen(header_name,"w+"); 
		
		DebugMessage("[dal_m3u8_http_request]: open header file headle%x!",headerfp);
		if(headerfp == NULL) 
		{
			DebugMessage("[dal_m3u8_http_request]:open head file error\n");
			fclose(filefp);
			
			DebugMessage("[dal_m3u8_http_request]: close file headle%x!-%d",filefp,__LINE__);
			curl_easy_cleanup(curl);
			
			DAL_SemSignal(g_M3u8_Info.sem_request);
			return -1;
		}

		/*set URL for a link*/
		curl_easy_setopt(curl, CURLOPT_URL,URL);

		curl_easy_setopt (curl, CURLOPT_HEADER,0); 


		/*set callback function and filepointer for the download file*/
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_fwrite);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, filefp);
		/*set callback function and filepointer for the http header*/
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, file_fwrite);	
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, headerfp);

		curl_easy_setopt(curl,CURLOPT_TIMEOUT,30);		//operate outtime is 5 sencond
		
		res = curl_easy_perform(curl);
		fclose(headerfp);
		fclose(filefp);
		
		DebugMessage("[dal_m3u8_http_request]: close file headle%x!-%d",filefp,__LINE__);	
		DebugMessage("[dal_m3u8_http_request]: close file headle%x!-%d",headerfp,__LINE__);
		
		DAL_SemSignal(g_M3u8_Info.sem_request);
		
		curl_easy_cleanup(curl);
		return res;
}

/*------------------------------------------------------------------*/
/* 0: 成功														*/
/* -1: 队列创建失败													*/
/* -1: task创建失败													*/
/*------------------------------------------------------------------*/
S32 DAL_M3u8TaskCreate(void)
{
	DAL_M3u8Control_t*		p_control = &g_M3u8Control;
	int						iret = 0;

	iret = DAL_MessageQueueCreate( &(p_control->queueID), "dal_M3U8_queue", DAL_M3U8_QUEUE_DEPTH, DAL_M3U8_QUEUE_DATA_SIZE, 0);
	if( iret != 0)
	{
		return -1;
	}
	iret = DAL_TaskCreate( &(p_control->taskID), "dal_HLS_task", dal_m3u8_request_task, NULL, NULL, DAL_M3U8_TASK_STACK_SIZE, DAL_M3U8_TASK_PRIOITY, 0);
	if( iret != 0)
	{
		DAL_MessageQueueDelete( p_control->queueID);
		return -2;
	}

	p_control->status = M3U8DOWN_STOP;
	return 0;
}

/*------------------------------------------------------------------*/
/* 0: 成功														*/
/* -1: 队列创建失败													*/
/* -1: task创建失败													*/
/*------------------------------------------------------------------*/
int DAL_M3u8TaskDelete( void)
{
	DAL_M3u8Control_t*		p_control = &g_M3u8Control;
	if( p_control->status == M3U8DOWN_IDEL)
	{
		DebugMessage("[dal]DAL_M3u8TaskDelete ->M3U8DOWN_IDEL");
		return -1;
	}
	DAL_TaskDelete( p_control->taskID);
	DAL_MessageQueueDelete( p_control->queueID);
	p_control->status = M3U8DOWN_IDEL;
	return 0;
}

DAL_QueueID_t DAL_GetM3u8QueueID(void)
{
	DAL_M3u8Control_t*		p_control = &g_M3u8Control;

	if( p_control->status == M3U8DOWN_IDEL)
	{
		DebugMessage("CORE_IDEL\ns");
		return -1;
	}
	
	return p_control->queueID;
}

static void init_dal_hls(void)
{
	PV_INIT_LIST_HEAD(&g_M3u8_Info.tM3U8_HEADER);
	g_M3u8_Info.catche = 0; 			/*0:不可以缓存，1:可以缓存*/
	g_M3u8_Info.live_state = 0; 		/*0:live stream; 1:no live stream*/ 
	g_M3u8_Info.discontinuity = 0;	/*0:no continuity stream; 1:continuity	stream*/	
 	g_M3u8_Info.duration = 0;
 	g_M3u8_Info.sequence = 0;
 	g_M3u8_Info.xkey = 0;
	g_M3u8_Info.total_num =0;
	
 	memset(g_M3u8_Info.program_id,0,sizeof(g_M3u8_Info.program_id));
	memset(g_M3u8_Info.bandwidth,0,sizeof(g_M3u8_Info.bandwidth));
	
	DAL_SemCreate(&g_M3u8_Info.sem_request, "dal m3u8 struct sem", 1, 0);
}

/*------------------------------------------------------------------*/
/* 1: 存在														*/
/* 0: 不存在													*/
/*------------------------------------------------------------------*/
static S32 check_hls_exit(Current_hls_t hlsnode)
{	
	struct pvlist_head *PvlistHeader=NULL,*pNode=NULL;
	M3u8_Addr_Node_t *AddressNode = NULL;

	
	DebugMessage("check_hls_exit !!!!");

	PvlistHeader = &g_M3u8_Info.tM3U8_HEADER;
	pvlist_for_each(pNode, PvlistHeader)
	{
		DebugMessage("[check_hls_exit] pvlist_for_each %x!",AddressNode);
	
		AddressNode = pvlist_entry(pNode,M3u8_Addr_Node_t,tNode);
		if(strncmp(AddressNode->http_address,hlsnode.http_address,255)==0)
		{
			return 1;
		}
	}
	
	return 0;

}


/*------------------------------------------------------------------*/
/* 0: 成功														*/
/* -1: 失败													*/
/*------------------------------------------------------------------*/
static S32 add_one_hls(Current_hls_t hlsnode)
{
	struct pvlist_head *PvlistHeader;
	M3u8_Addr_Node_t *Newnode;

	
	DebugMessage("add_one_hls !!!!");

	PvlistHeader = &g_M3u8_Info.tM3U8_HEADER;
	Newnode = DAL_Malloc(sizeof(M3u8_Addr_Node_t));
	if(Newnode == NULL)
	{
		DebugMessage("[add_one_hls] malloc error!");
		return -1;
	}
	DebugMessage("**************************************");
	
	memset(Newnode,0,sizeof(M3u8_Addr_Node_t));
	strcpy(Newnode->http_address,hlsnode.http_address);
	
	DebugMessage("add_one_hls address:%s ",Newnode->http_address);
	Newnode->segement_info = hlsnode.segement_info;	
	pvlist_add_tail(&Newnode->tNode, PvlistHeader);
	g_M3u8_Info.total_num++;
	
	return 0;

}

/*------------------------------------------------------------------*/
/* 0: 成功														*/
/* -1: 失败													*/
/*------------------------------------------------------------------*/
static S32 del_one_hls(Current_hls_t* hlsnode)
{
	struct pvlist_head *PvlistHeader=NULL,*pNode=NULL;
	M3u8_Addr_Node_t *AddressNode = NULL;
	U8 find_addr = 0;
	S32 sRet = 0;

	
	PvlistHeader = &g_M3u8_Info.tM3U8_HEADER;
	DAL_SemWait(g_M3u8_Info.sem_request);

	pvlist_for_each(pNode, PvlistHeader)
	{
		DebugMessage("[del_one_hls] pvlist_for_each %x!",AddressNode);
	
		AddressNode=pvlist_entry(pNode,M3u8_Addr_Node_t,tNode);
		if(strncmp(AddressNode->http_address,hlsnode->http_address,255)==0)
		{
			find_addr=1;
			break;
		}
	}
	if(find_addr==1)
	{
	
		pvlist_del(pNode);
		
		DebugMessage("[del_one_hls] pvlist_del!");
		DAL_Free(AddressNode);
		//g_M3u8_Info.total_num--;
		DebugMessage("[del_one_hls] DAL_Free!");
		DAL_SemSignal(g_M3u8_Info.sem_request);
		return 0;
	}
	DebugMessage("[del_one_hls]  error,not find the address!");
	DAL_SemSignal(g_M3u8_Info.sem_request);
	return -1;

}

static void del_all_hls(void)
{
	struct pvlist_head *PvlistHeader,*pdelNode,*pNode;
	M3u8_Addr_Node_t *AddressNode;

	
	//DAL_SemWait(g_M3u8_Info.sem_request);
	PvlistHeader = &g_M3u8_Info.tM3U8_HEADER;
	pdelNode = PvlistHeader->next;

	while(pdelNode != PvlistHeader)
	{
		pNode = pdelNode->next;
		AddressNode=pvlist_entry(pdelNode,M3u8_Addr_Node_t,tNode);
		pvlist_del(pdelNode);
		DAL_Free(AddressNode);
		pdelNode = pNode;
	}
	g_M3u8_Info.total_num =0;
#if 0	
	g_M3u8_Info.catche = 0; 			/*0:不可以缓存，1:可以缓存*/
	g_M3u8_Info.live_state = 0; 		/*0:live stream; 1:no live stream*/ 
	g_M3u8_Info.discontinuity = 0;	/*0:no continuity stream; 1:continuity	stream*/	
 	g_M3u8_Info.duration = 0;
 	g_M3u8_Info.sequence = 0;
 	g_M3u8_Info.xkey = 0;

	
 	memset(g_M3u8_Info.program_id,0,sizeof(g_M3u8_Info.program_id));
	memset(g_M3u8_Info.bandwidth,0,sizeof(g_M3u8_Info.bandwidth));
#endif	
//	DAL_SemSignal(g_M3u8_Info.sem_request);

}


static void m3u8_download_queue_clear( void)
{
	DAL_M3u8Control_t*		p_control = &g_M3u8Control;
	int							iret = 0;
	DAL_Message_t*				pMsg = NULL;
	U32							SizeCopy = 0;
	U32							msg_clear = 0;

	DebugMessage("[m3u8_download_queue_clear]entry\n");

	while( (iret = DAL_MessageQueueReceive( p_control->queueID, &pMsg, sizeof(DAL_Message_t*), &SizeCopy, DAL_CHECK)) >= 0)
	{
		msg_clear++;
		DAL_DEBUG(("clear a message  %s    %d\n", DAL_MessageString( pMsg->MsgType),iret));
		DAL_MessageFree(pMsg);
	}
	DebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^[m3u8_download_queue_clear]  clear %d message!!!\n", msg_clear);

}


S32 DAL_M3u8DownloadStart(M3u8_address_t* param)
{
	DAL_DownloadControl_t*	down_control = &g_DownloadControl;

	DAL_M3u8Control_t*		p_control = &g_M3u8Control;
	DAL_Message_t*				pMsg = NULL;

	DebugMessage("[dal]DAL_M3u8DownloadStart ->entry");

	if( p_control->status == M3U8DOWN_IDEL)
	{
		DebugMessage("[dal]DAL_M3u8DownloadStart ->DOWNLOAD_IDEL");
		return -1;
	}


	if( ring_buffer_is_init( &(down_control->download_buffer)) == 0)
	{
		DebugMessage("[dal]parse_real_httpaddress -> DOWNLOAD BUFFER NOT INIT\n");
		return -1;
	}
	ring_buffer_flush(&(down_control->download_buffer));
	
	init_dal_hls();

	/* 清空可能堆积的消息*/
	m3u8_download_queue_clear(); 
	download_queue_clear();

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_M3U8_ADDRESSDOWNLOAD_START;
	memset( pMsg->MsgData.DownloadStart.url, 0, sizeof(pMsg->MsgData.DownloadStart.url));
	strcpy( pMsg->MsgData.DownloadStart.url, param->Url);
	DebugMessage("********%s****",pMsg->MsgData.StreamParam.input.Source.http.URL);
	DAL_MessageSend( p_control->queueID, &pMsg);
	
	//nlv
	 down_control->parse_pids_flag = PPF_NO_REQUEST;
	
	p_control->status = M3U8DOWN_START;
	return 0;
}

int dal_M3u8_request_is_stop( void)
{
	DAL_DownloadControl_t*		p_control = &g_M3u8Control;
	return (p_control->status == M3U8DOWN_STOP)?1:0;
}

S32 DAL_M3u8DownloadStop(void)
{
	DAL_M3u8Control_t*	   down_control = &g_M3u8Control;
	DAL_Message_t*				pMsg = NULL;

	if( down_control->status == M3U8DOWN_IDEL || down_control->status == M3U8DOWN_STOP)
	{
		DebugMessage("[dal]DAL_M3u8DownloadStop->M3U8DOWN_IDEL or M3U8DOWN_STOP");
		return -1;
	}

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_M3U8_REQUEST_STOP;
	//DAL_MessageSend( down_control->queueID, &pMsg);
	stop_request_m3u8( pMsg);


	while( dal_M3u8_request_is_stop() != TRUE)
	{
		DebugMessage("[dal]DAL_M3u8DownloadStop->waiting for DAL_M3u8DownloadStop");
		DAL_TaskDelay(500);
	}
		
	return 0;
}

static void* dal_m3u8_request_task(void* param)
{
	DAL_M3u8Control_t*		p_control = &g_M3u8Control;
	DAL_Message_t*		pMsg = NULL;
	U32 				SizeCopy;
	int 				iret = 0;

	while(1)
	{
		iret = DAL_MessageQueueReceive( p_control->queueID, &pMsg, sizeof(DAL_Message_t*), &SizeCopy, DAL_PEND);
		if( iret < 0)
		{
			DebugMessage("[DAL]dal_hls_request_task ->   DAL_MessageQueueReceive no message\n");
			continue;
		}
		else
		{
		
			DebugMessage("[DAL]dal_m3u8_request_task -> 	 Recv MSG: %s	%d\n", DAL_MessageString( pMsg->MsgType), iret);
			switch( pMsg->MsgType)
			{
				case DALMSG_M3U8_ADDRESSDOWNLOAD_START: get_real_httpaddress( pMsg); break;
				
				case DALMSG_FIRST_M3U8PARSE_START: parse_firstlevel_m3u8( pMsg); break;
				case DALMSG_SECOND_M3U8PARSE_START: parse_secondlevel_m3u8(pMsg); break;
				case DALMSG_M3U8_REQUEST_STOP: stop_request_m3u8(pMsg); break;
				
				case DALMSG_M3U8_TSDOWNLOAD_START: ts_download_start(pMsg); break;				
				case DALMSG_M3U8_TSDOWNLOAD_STOP: ts_download_stop(pMsg); break;
			//	case DALMSG_M3U8_RESUME_PARSE:HLS_resume_parse(pMsg);break;
				default: break;
			}
			DAL_MessageFree(pMsg);
		}
		DAL_TaskDelay(100);
	}
	return (void*)0;
}


static void get_real_httpaddress( DAL_Message_t* pMsg)
{
	DAL_M3u8Control_t*		p_control = &g_M3u8Control;
	DAL_Message_t*		p_msg = NULL;
	S32					iret = 0;
	static S32 			timeout_error=0;
	C8					url[256],geturl[256];
	dal_evt_param_t				dal_evt_param;

	
	if( p_control->status == M3U8DOWN_IDEL)
	{
		DebugMessage("[dal]parse_real_httpaddress ->idel\n");
		return ;
	}
	
	DebugMessage("[dal]parse_real_httpaddress -> DOWNLOAD start url:%s %d\n",pMsg->MsgData.DownloadStart.url,strlen(pMsg->MsgData.DownloadStart.url));

	memset(url,0,sizeof(url));
	strcpy( url, pMsg->MsgData.DownloadStart.url);

	{
		iret = dal_m3u8_http_request(url,HLS_HeadName,HLS_FileName);
		if(iret!= 0)
		{
			DebugMessage("[dal]parse_real_httpaddress ->down load error %d !",iret);
			if(iret == 28)
			{
				DAL_Message_t*		pMsg = NULL;

				if(timeout_error<=3)
				{
					timeout_error++;
				pMsg = DAL_MessageAlloc();
				if( NULL != pMsg){
					pMsg->MsgType = DALMSG_M3U8_ADDRESSDOWNLOAD_START;
					memset( pMsg->MsgData.DownloadStart.url, 0, sizeof(pMsg->MsgData.DownloadStart.url));
					strcpy( pMsg->MsgData.DownloadStart.url,  url);
					DAL_MessageSend( DAL_GetM3u8QueueID(), &pMsg);
				}else{
					DebugMessage("DAL_MessageAlloc failed 888 \n");
				}
			}
				else
				{	
					p_control->status = M3U8DOWN_STOP;
					dal_core_event_notify(DAL_CORE_EVENT_M3U8_DOWNLOAD_BAD_STATUS,&dal_evt_param);
					DebugMessage("dal_m3u8_http_request error28  times >3");
				
				}

			}
			else
			{
				p_control->status = M3U8DOWN_STOP;
				dal_core_event_notify(DAL_CORE_EVENT_M3U8_DOWNLOAD_BAD_STATUS,&dal_evt_param);
				//libcurl 连接不上了，建议上层收到消息退出
				DebugMessage("dal_m3u8_http_request error ,but 28");
				
			}
		
			return;
		}
		timeout_error=0;
		memset(geturl,0,sizeof(geturl));
		iret=dal_parse_first_http_header(HLS_HeadName,geturl);
		if(iret==0)/*解析下载文件*/
		{
			DAL_Message_t*		pMsg = NULL;
			trynum=0;
			
			pMsg = DAL_MessageAlloc();
			if( NULL != pMsg){
				pMsg->MsgType = DALMSG_FIRST_M3U8PARSE_START;
				DAL_MessageSend( DAL_GetM3u8QueueID(), &pMsg);
			}else{
				DebugMessage("DAL_MessageAlloc failed 1 \n");
			}
		}
		else if(iret == 1)/*可以用location再次请求*/
		{
			DAL_Message_t*		pMsg = NULL;
			trynum=0;
			
			if(geturl[0]!=0)
			{
				memset(hls_http,0,sizeof(hls_http));
				strcpy(hls_http,geturl);
			}
			pMsg = DAL_MessageAlloc();
			if( NULL != pMsg){
				pMsg->MsgType = DALMSG_M3U8_ADDRESSDOWNLOAD_START;
				memset( pMsg->MsgData.DownloadStart.url, 0, sizeof(pMsg->MsgData.DownloadStart.url));
				strcpy( pMsg->MsgData.DownloadStart.url, geturl);
				DAL_MessageSend( DAL_GetM3u8QueueID(), &pMsg);
			}else{
				DebugMessage("DAL_MessageAlloc failed 2 \n");
			}
		}
		else if((iret == -1)||(iret == -2))/*解析头失败*/
		{
			trynum++;
			/*失败后多请求一次*/
			if(trynum <= 5 )
			{
				DAL_Message_t*		pMsg = NULL;

				pMsg = DAL_MessageAlloc();
				if( NULL != pMsg){
					pMsg->MsgType = DALMSG_M3U8_ADDRESSDOWNLOAD_START;
					memset( pMsg->MsgData.DownloadStart.url, 0, sizeof(pMsg->MsgData.DownloadStart.url));
					strcpy( pMsg->MsgData.DownloadStart.url,  url);
					DAL_MessageSend( DAL_GetM3u8QueueID(), &pMsg);
				}else{
					DebugMessage("DAL_MessageAlloc failed 3 \n");
				}
			}
			else
			{
				p_control->status = M3U8DOWN_STOP;
				dal_core_event_notify(DAL_CORE_EVENT_M3U8_DOWNLOAD_BAD_STATUS,&dal_evt_param);
				DebugMessage("try 6 times failed,not try again\n");
				trynum=0;
				return;
			}
		}
	}	
}

static void stop_request_m3u8( DAL_Message_t*  pMsg)
{
	DAL_M3u8Control_t*		p_control = &g_M3u8Control;

	DebugMessage("[dal]stop_request_m3u8 ->enter!:%d",p_control->status);

	if( p_control->status == M3U8DOWN_START)
	{
		p_control->status = M3U8DOWN_STOP;
	}
	else if( p_control->status == M3U8DOWN_STOP)
	{
		DebugMessage("[dal]stop_request_m3u8 ->ALREADY REQUEST_STOP");
	}
}

static void parse_firstlevel_m3u8(DAL_Message_t* pMsg)
{
	DAL_M3u8Control_t*		p_control = &g_M3u8Control;
	dal_evt_param_t				dal_evt_param;
	S32 iret=0;

	
	if( p_control->status == M3U8DOWN_STOP)
	{
		DebugMessage("[dal]parse_firstlevel_m3u8 ->stop\n");
		return ;
	}
	iret = dal_parse_first_level_m3u8(HLS_FileName);
	
	if(iret == 1)
	{
		DAL_Message_t*		pMsg = NULL;
		
		pMsg = DAL_MessageAlloc();
		if( NULL != pMsg){
			pMsg->MsgType = DALMSG_SECOND_M3U8PARSE_START;
			DAL_MessageSend( DAL_GetM3u8QueueID(), &pMsg);
		}else{
			DebugMessage("[dal]parse_firstlevel_m3u8 -> DAL_MessageAlloc failed 1 \n");
		}
		return;
	}
	else if(iret ==0)
	{
	
		DAL_Message_t*		Msg = NULL;
		DebugMessage("[dal]find firstlevel_m3u8 OK! \n");
		
		Msg = DAL_MessageAlloc();
			if( NULL != Msg){
				Msg->MsgType = DALMSG_M3U8_ADDRESSDOWNLOAD_START;
				memset( Msg->MsgData.DownloadStart.url, 0, sizeof(Msg->MsgData.DownloadStart.url));
				strcpy( Msg->MsgData.DownloadStart.url,hls_http);
				DAL_MessageSend(DAL_GetM3u8QueueID(), &Msg);
			}else{
				DebugMessage("[dal]ts_download_start -> DAL_MessageAlloc failed 2 \n");
			}
		/*暂时不做解析*/
	}
	else if	(iret == -1)
	{
		p_control->status = M3U8DOWN_STOP;
		dal_core_event_notify(DAL_CORE_EVENT_M3U8_DOWNLOAD_BAD_STATUS,&dal_evt_param);
		DebugMessage("[dal]parse_firstlevel_m3u8 failed! \n");
	
	}
	
}

static void parse_secondlevel_m3u8( DAL_Message_t* pMsg)
{
	DAL_M3u8Control_t*		p_control = &g_M3u8Control;
	dal_evt_param_t				dal_evt_param;
	DAL_Message_t*		Msg = NULL;
	S32 iret=0;

	
	if( p_control->status == M3U8DOWN_STOP)
	{
		DebugMessage("[dal]parse_secondlevel_m3u8 ->stop\n");
		return ;
	}
	iret = dal_parse_second_level_m3u8(HLS_FileName);

	if(iret ==0)
	{
		
		DebugMessage("[dal]parse_secondlevel_m3u8 OK! \n");
		
		/*发消息下载ts*/
		Msg = DAL_MessageAlloc();
		if( NULL != Msg){
			Msg->MsgType = DALMSG_M3U8_TSDOWNLOAD_START;
			DAL_MessageSend(DAL_GetM3u8QueueID(), &Msg);
		}else{
			DebugMessage("[dal]parse_secondlevel_m3u8 -> DAL_MessageAlloc failed  \n");
			return;
		}

	}
	else if (iret == -1)
	{
	
		p_control->status = M3U8DOWN_STOP;
		dal_core_event_notify(DAL_CORE_EVENT_M3U8_DOWNLOAD_BAD_STATUS,&dal_evt_param);
		DebugMessage("[dal]parse_secondlevel_m3u8 failed! \n");		
		return;
	}
	
}



static void ts_download_start(DAL_Message_t* pMsg)
{
	struct pvlist_head   *tTemp = &g_M3u8_Info.tM3U8_HEADER;
	 M3u8_Addr_Node_t *Anode;
	 DAL_Message_t* 	 Msg = NULL;
	// Current_hls_t		tempnode;
	 dal_evt_param_t			 dal_evt_param;
	 S32		iret=0;
	 
	 DebugMessage("[ts_download_start]entry total_num:%d,num:%d\n",g_M3u8_Info.total_num,num);
	 DebugMessage("[ts_download_start]entry total_num:%d,num:%d\n",g_M3u8_Info.total_num,num);
	 DebugMessage("[ts_download_start]entry total_num:%d,num:%d\n",g_M3u8_Info.total_num,num);
	 DebugMessage("[ts_download_start]entry total_num:%d,num:%d\n",g_M3u8_Info.total_num,num);
	 DebugMessage("[ts_download_start]entry total_num:%d,num:%d\n",g_M3u8_Info.total_num,num);
	 DebugMessage("[ts_download_start]entry total_num:%d,num:%d\n",g_M3u8_Info.total_num,num);
	if(g_M3u8Control.status==M3U8DOWN_STOP)
	{
		return;
	}
	if(num < g_M3u8_Info.total_num)
	{
		int i=0;
		for(i=0;i<=num;i++)
		{
			tTemp = tTemp->next;
		}
		Anode = pvlist_entry(tTemp, M3u8_Addr_Node_t, tNode);
		num++;
		Msg = DAL_MessageAlloc();
		if( NULL != Msg)
		{
			Msg->MsgType = DALMSG_M3U8_TSDOWNLOAD;
			memset( Msg->MsgData.DownloadStart.url, 0, sizeof(Msg->MsgData.DownloadStart.url));
			strcpy( Msg->MsgData.DownloadStart.url,Anode->http_address);
	//		set_segement_time(Anode->segement_info);
			DebugMessage("[ts_download_start]entry url:%s\n",Msg->MsgData.DownloadStart.url);
			DAL_MessageSend(DAL_GetDownloadQueueID(), &Msg);
		//	memset(&tempnode,0,sizeof(Current_hls_t));
		//	tempnode.segement_info=Anode->segement_info;
		//	strcpy( tempnode.http_address,Anode->http_address);
			
			DebugMessage("[ts_download_start]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
		//	iret=del_one_hls(&tempnode);
		//	if(iret!=0)
		//		DebugMessage("[dal]del_one_hls -> failed \n");

		}
		else
		{
			DebugMessage("[dal]ts_download_start -> DAL_MessageAlloc failed 1 \n");
		}
	}
	else
	{
		if(g_M3u8_Info.live_state==0)
		{
			Msg = DAL_MessageAlloc();
			if( NULL != Msg){
				Msg->MsgType = DALMSG_M3U8_ADDRESSDOWNLOAD_START;
				memset( Msg->MsgData.DownloadStart.url, 0, sizeof(Msg->MsgData.DownloadStart.url));
				strcpy( Msg->MsgData.DownloadStart.url,hls_http);
				DAL_MessageSend(DAL_GetM3u8QueueID(), &Msg);
			}else{
				DebugMessage("[dal]ts_download_start -> DAL_MessageAlloc failed 2 \n");
			}
		}
		else
		{
			Msg = DAL_MessageAlloc();
			if( NULL != Msg)
			{
				Msg->MsgType = DALMSG_STREAM_STOP;
				DAL_MessageSend( DAL_GetCoreQueueID(), &Msg);
				
				dal_core_event_notify(DAL_CORE_EVENT_PLAYEND,&dal_evt_param);
			}
			else
			{
				DebugMessage("[dal]ts_download_start -> DAL_MessageAlloc failed 3\n");
			}
		}
	}
	
	 DebugMessage("[m3u8_perpare_parse] 2--> p_control %d ",get_core_controlstatus());
	if(get_core_controlstatus() == 5||get_core_controlstatus() == 4)
		 m3u8_perpare_parse();
	
	DebugMessage("[m3u8_perpare_parse] 2-->  p_control %d ",get_core_controlstatus());
	
}

S32 ts_download_stop(void)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				pMsg = NULL;
	dal_evt_param_t				dal_evt_param;	
	C8 str[256] = {0};


	if( p_control->status == DOWNLOAD_IDEL)
	{
		DebugMessage("[dal]DAL_DownloadStop ->DOWNLOAD_IDEL");
		dal_core_event_notify(DAL_CORE_EVENT_HLS_STOP_BAD_STATUS,&dal_evt_param);
		return -1;
	}	

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_M3U8_TSSTOP;
	DAL_MessageSend( p_control->queueID, &pMsg);

	while( DAL_DownloadIsStop() != TRUE)
	{
		DebugMessage("[dal]ts_DownloadStop ->waiting for ts_DownloadStop");
		DAL_TaskDelay(200);
	}
	
	DAL_SemWait(g_M3u8_Info.sem_request);
	del_all_hls();
	
	sprintf(str,"rm %s",HLS_HeadName);
	system(str);
	sprintf(str,"rm %s",HLS_FileName);
	system(str);
	
	DAL_SemSignal(g_M3u8_Info.sem_request);
	DAL_SemDelete(g_M3u8_Info.sem_request);
	num=0;
	trynum=0;
	set_hls_resume_state(0);
	return 0;
}

U8 get_live_state(void)
{

return g_M3u8_Info.live_state;

}
U8 get_catch_state(void)
{

	return g_M3u8_Info.catche;
}


/***************************************************************/
/***************************************************************/
void test_hls(void)
{
	S32				i=0,	iret = 0;

	char *url ="http://192.168.0.201/hls_server/hls_server_test/output_media/harrypotter.m3u8";
for(i=0;i<200000;i++)
{
	iret = dal_m3u8_http_request(url,HLS_HeadName,HLS_FileName);
	DebugMessage("[dal]test_hls ->dal_m3u8_http_request error= %d ,i=%d",iret,i);
	DAL_TaskDelay(30);
}
DebugMessage("[dal]test_hls ->over");
}

void Set_Http_header_addr(C8* cstring)
{
	memset(HLS_HeadName,0,sizeof(HLS_HeadName));
	strcpy(HLS_HeadName,cstring);
}

void Set_Http_File_addr(C8* cstring)
{
	memset(HLS_FileName,0,sizeof(HLS_FileName));
	strcpy(HLS_FileName,cstring);
}

U32 Get_HLS_file_info(void)
{
	U32  total=0;
	total=g_M3u8_Info.total_time*1000;
	
	pbiinfo(" get hls film total time :%d ms",total);
	return total;
}

S32 HLS_pause( void )
{
	dal_evt_param_t				dal_evt_param;
	S32							iret = 0;
	DebugMessage("[dal]HLS HLS_pause -> ->  -> ->  -> ->  -> -> entry");

	
	if(g_M3u8_Info.catche !=1)
	{
		DebugMessage("[HLS_resume]HLS no catche state ,not support");
		dal_core_event_notify(DAL_CORE_EVENT_HLS_PAUSE_RESUME_NOT_SUPPORT,&dal_evt_param);
		return -1;
	}

	hls_movie_playtime=Get_play_time();

	iret =DAL_InjectPause();
	if( iret != 0)
	{
		DebugMessage("[dal]HLS_pause -> -> DAL_InjectPause failed");
		dal_core_event_notify( DAL_CORE_EVENT_PAUSE_FAILED, &dal_evt_param);
		return -1;
	}
	
	pbiinfo("[dal]HLS_pause -> -> hls now play time [%d]",hls_movie_playtime);

	
	iret =  M3u8DownloadPause();
	if( iret != 0)
	{
		DebugMessage("[dal]HLS HLS_pause -> -> M3u8DownloadPause failed");
		dal_core_event_notify( DAL_CORE_EVENT_PAUSE_FAILED, &dal_evt_param);
		return -1;
	}
	iret = DAL_DownloadBufferRelease();
	if( iret != 0)
	{
		DebugMessage("[dal]HLS_pause ->	-> DAL_DownloadBufferRelease failed");		
		dal_core_event_notify( DAL_CORE_EVENT_PAUSE_FAILED, &dal_evt_param);
		return -1;
	}	
	
	iret = DAL_ParseStop();
	if( iret != 0)
	{
		DebugMessage("[dal]HLS_pause ->	-> DAL_ParseStop failed");		
		dal_core_event_notify( DAL_CORE_EVENT_PAUSE_FAILED, &dal_evt_param);
		return -1;
	}
	
	iret=DRV_AVCtrl_Set_PlayPause();
	if( iret != 0)
	{
		DebugMessage("[dal]HLS_pause -> -> DRV_AVCtrl_Set_PlayPause failed");
		dal_core_event_notify( DAL_CORE_EVENT_PAUSE_FAILED, &dal_evt_param);
		return -1;
	}
	
	
	
	SetCoreStatus(1);
	dal_core_event_notify( DAL_CORE_EVENT_PAUSE_OK, &dal_evt_param);

	return 0;
}
S32 HLS_resume( char *url )
{
	dal_evt_param_t				dal_evt_param;
	S32							iret = 0;
	DAL_Message_t*				pMsg = NULL;
	DebugMessage("[dal]HLS HLS_resume -> ->  -> ->  -> ->  -> -> entry");

	if(g_M3u8_Info.catche !=1)
	{
		DebugMessage("[HLS_resume]HLS no catche state ,not support");
		dal_core_event_notify(DAL_CORE_EVENT_HLS_PAUSE_RESUME_NOT_SUPPORT,&dal_evt_param);
		return -1;
	}

	set_hls_resume_state(1);
	/*frist time to download data , alloc memory for download buffer*/
	iret = DAL_DownloadBufferInit();
	if( iret != 0)
	{
		DebugMessage("[HLS_resume]P2V_resume->DAL_DownloadBufferInit failed\n");
		dal_core_event_notify(DAL_CORE_EVENT_P2V_RESUME_DL_BUF_ALLOC_FAILED,&dal_evt_param);
		return -1;
	}

	iret = M3u8DownloadResume();
	if( iret != 0)
	{
		DebugMessage("[HLS_resume]HLS_resume->DAL_DownloadBufferInit failed\n");
		dal_core_event_notify(DAL_CORE_EVENT_P2V_RESUME_DL_START_FAILED,&dal_evt_param);
		return -1;
	}

	DebugMessage("[DAL]HLS_resume OK");


	/* set status CORE_STREAM_WAIT_PID, when patse task get new pids and notify core task, program will be play start*/
	SetCoreStatus(0);
	//dal_core_event_notify(DAL_CORE_EVENT_STATUS_WIAT_PIDS,&dal_evt_param);

	return 0;
}
void HLS_resume_parse(void)
{
	if(get_core_controlstatus() == 4)
		 m3u8_perpare_parse();

}
void seek_offset_time(U32* seg_nun,U32*seg_pasttime)
{
	
	struct pvlist_head *PvlistHeader=NULL,*pNode=NULL;
	M3u8_Addr_Node_t *AddressNode = NULL;
	U32 			i_num=0,playtime=hls_movie_playtime/1000;

	PvlistHeader = &g_M3u8_Info.tM3U8_HEADER;
	
	i_num=0;
	pvlist_for_each(pNode, PvlistHeader)
	{
		AddressNode = pvlist_entry(pNode,M3u8_Addr_Node_t,tNode);


		DebugMessage("[check_hls_exit] seek_offset_time %d-%d!%d",playtime,AddressNode->segement_info,i_num);
		if( playtime >= AddressNode->segement_info)
		{
			playtime = playtime - AddressNode->segement_info;
			
		}
		else 
		{
			*seg_nun=i_num;
			*seg_pasttime=playtime;
			break;

		}
		i_num++;
	}
}

S32 M3u8DownloadPause(void)
{
	DAL_M3u8Control_t*	   down_control = &g_M3u8Control;
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				pMsg = NULL;
	dal_evt_param_t 			dal_evt_param;	

	if( down_control->status == M3U8DOWN_IDEL || down_control->status == M3U8DOWN_STOP)
	{	
		dal_core_event_notify(DAL_CORE_EVENT_HLS_STOP_BAD_STATUS,&dal_evt_param);
		DebugMessage("[dal]M3u8DownloadPause--->M3U8DOWN_IDEL or M3U8DOWN_STOP");
		return -1;
	}
	
	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_M3U8_REQUEST_STOP;
	stop_request_m3u8( pMsg);
	while( dal_M3u8_request_is_stop() != TRUE)
	{
		DebugMessage("[dal]M3u8DownloadPause->waiting for DAL_M3u8DownloadStop");
		DAL_TaskDelay(500);
	}
		

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_M3U8_TSSTOP;
	DAL_MessageSend( p_control->queueID, &pMsg);

	while( DAL_DownloadIsStop() != TRUE)
	{
		DebugMessage("[dal]M3u8DownloadPause ->waiting for ts_DownloadStop");
		DAL_TaskDelay(200);
	}
	set_hls_resume_state(0);
	
	return 0;
}

S32 M3u8DownloadResume(void)
{
	DAL_M3u8Control_t*	   		down_control = &g_M3u8Control;
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	dal_evt_param_t 			dal_evt_param;	
	U32 segment_time=0,segment_num=0;

	if((down_control->status == M3U8DOWN_STOP)&&(p_control->status == DOWNLOAD_STOP))
	{
		ring_buffer_flush(&(p_control->download_buffer));

		m3u8_download_queue_clear(); 
		download_queue_clear();
		p_control->parse_pids_flag = PPF_NO_REQUEST;
		down_control->status = M3U8DOWN_START;
		
		seek_offset_time(&segment_num,&segment_time);
		num=segment_num;
		ts_download_start(NULL);
		
		DebugMessage("[dal]M3u8DownloadResume--->%d",num);
		return 0;
	}
	else
	{
		dal_core_event_notify(DAL_CORE_EVENT_HLS_STOP_BAD_STATUS,&dal_evt_param);
		DebugMessage("[dal]M3u8DownloadResume--->M3U8DOWN_IDEL or M3U8DOWN_START");
		return -1;
	}

}

void set_hls_resume_state(U8 flag)
{
	HLS_resume_flag=flag;


}
U8 Get_hls_resume_state(void)
{
	return HLS_resume_flag;

}

U32 Get_hls_pause_time(void)
{
	return hls_movie_playtime;
	
}
void Set_hls_pause_time(U32 value)
{
	 hls_movie_playtime=value;
	
}


