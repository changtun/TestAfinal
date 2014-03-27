#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl.h>
#include <types.h>
#include <easy.h>

#include "expat.h"

#include "osapi.h"
#include "pvddefs.h"

#include "dal_wrap.h"
#include "dal_common.h"


#define HTTP_RESPONSE_SIZE		512



typedef struct httpResult_s
{
	char		data[HTTP_RESPONSE_SIZE];
	int			size;
}httpResult_t;

static FILE* temp_fp = NULL;

static char* string_convert(const char* string);


static size_t writeFunc(void *buffer, size_t size, size_t nmemb, void *stream)
{
	httpResult_t* p_result = (httpResult_t*)stream;


	if( p_result->size + size*nmemb > HTTP_RESPONSE_SIZE){
		//printf("writeFunc: p_result->size: %u   size: %u nmemb: %u \n", p_result->size, size, nmemb);	
		DebugMessage("[DAL] writeFunc: p_result->size: %u   size: %u nmemb: %u \n", p_result->size, size, nmemb);
		return -1;
	}

		/*{
			int ret = 0;
			ret = fwrite( buffer, size, nmemb, temp_fp);
			printf("fwrite: %u\n", ret);
			fclose(temp_fp);
			system("sync");
		}*/
	
	memcpy(p_result->data + p_result->size, buffer, size*nmemb);
	p_result->size += size*nmemb;
	return size*nmemb;
}



int httpRequest( const char* url, int timeout, httpResult_t* p_result)
{
	CURL*					curl_handle;
	CURLcode				curl_error;

	assert( url && p_result);
		
	//DAL_DEBUG(("url: %s\n",url));
	DebugMessage("[DAL]httpRequest url: %s\n",url);

	bzero(p_result,sizeof(httpResult_t));

	/*
	temp_fp = fopen( strrchr(url,'/')+1, "w");
	assert(temp_fp);
	*/
	
	curl_handle = curl_easy_init();
	curl_easy_setopt( curl_handle, CURLOPT_URL, url);
	curl_easy_setopt( curl_handle, CURLOPT_WRITEFUNCTION, writeFunc);
	curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, p_result);
	curl_easy_setopt( curl_handle, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt( curl_handle, CURLOPT_TIMEOUT, (long)timeout);
	curl_error = curl_easy_perform( curl_handle);
	curl_easy_cleanup(curl_handle);
	switch( curl_error){
		case CURLE_OK:{
			//DAL_DEBUG(("curl_easy_perform return CURLE_OK\n"));
			DebugMessage("curl_easy_perform return CURLE_OK\n");
			return 0;
		}
		break;
		default:{
			//DAL_DEBUG(("curl_easy_perform return %d  %s\n", curl_error, curl_easy_strerror(curl_error)));
			DebugMessage("curl_easy_perform return CURLE_OK\n");
			return -1;
		}
		break;
	}
}

//zhd Add+
int http_request_xml(char *buf){
	char *request_url = "http://218.241.131.179/SMSLogin.php?ProviderID=PBI1&MACKEY=MD5(MAC).com";
	//char *request_url = "http://218.241.131.190/SMSLogin.php?ProviderID=PBI1&MACKEY=MD5(MAC).com";
	//printf("======================%s===============\n", request_url);
	httpResult_t result;
	int				iret = 0;
	SYS_DEBUG(SD_DEBUG,MODULE_DAL,"request_url: %s\n",result.data);
	memset(&result,0,sizeof(httpResult_t));
	iret = httpRequest(request_url,10, &result);
	printf("|||||||||||||||||||||||||||||||| iret = %d\n", iret);
	if( iret != 0){
		SYS_DEBUG(SD_ERROR,MODULE_DAL,"httpRequest failed\n");
		return -1;
	}
	//printf("++++++++++++++++++++++++++++++++==result.data=%s\n", result.data);
	
	
	strcpy(buf, result.data);
	//printf("HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH=*buf=%sHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH\n", buf);
	return 0;
}

#ifdef MMS
static void XMLCALL xmlStartFunc(void *pUserData, const char *pElement, const char **pEleAttr)
{
	int i = 0;

	
	printf( "pElement:  %s \n",string_convert(pElement));
	
	if( '\0' != pEleAttr[0]){
		printf( "pEleAttr[0]:  %s \n",string_convert(pEleAttr[0]));
	}
	if( '\0' != pEleAttr[1]){
		printf( "pEleAttr[1]:  %s \n", string_convert(pEleAttr[1]));
	}

	//printf("%d %d \n", strcasecmp( string_convert(pElement), "REF"), strcasecmp(string_convert( pEleAttr[0]),"HREF"));
	
	if( strcasecmp( string_convert(pElement), "REF") == 0 
		&& strcasecmp( string_convert(pEleAttr[0]), "HREF") == 0)
	{
		
		printf(">>>>>>>>>>>>>>>>>>>>>>>>   need link: %s\n", string_convert(pEleAttr[1]));
		strcpy( (char*)pUserData, string_convert(pEleAttr[1]));
	}
	
}


static void XMLCALL xmlEndFunc(void *pUserData, const char *pElement)
{

}
static void XMLCALL xmlCharParseFunc(void *pUserData,const XML_Char *pXmlchar,int	siLen)
{

}



int parseAsx(const char* pData, int dataSize, char* actualUrl, int urlSize)
{
	XML_Parser 		parser = NULL;
	int				iret = 0;

	assert(pData && actualUrl);

	parser = XML_ParserCreate(NULL);
	if( parser == NULL ){
		DAL_ERROR(("XML_ParserCreate failed\n"));
		return -1;
	}

	XML_SetElementHandler( parser, xmlStartFunc, xmlEndFunc);
	XML_SetUserData( parser,actualUrl);
	XML_SetCharacterDataHandler( parser, xmlCharParseFunc);

	for(;;)
	{				
		if( XML_Parse( parser, pData, (XML_Size)dataSize, 0) == XML_STATUS_ERROR)
		{
			DAL_ERROR(("XML_Parse ERROR!!!  %s at line %d\n",XML_ErrorString(XML_GetErrorCode(parser)),(int)XML_GetCurrentLineNumber(parser)));
			iret =-1;
			break;
		}
	}	
	XML_ParserFree(parser);
	return 0;
}

int parseFile(void)
{
	XML_Parser 		parser = NULL;
	FILE*			fp = NULL;
	char			buffer[1024];
	int				file_length = 0;
	int				read_size;
	int				iret = 0;
//	int				i = 0;

	parser = XML_ParserCreate(NULL);
	if( parser == NULL )
	{
		DAL_ERROR(("XML_ParserCreate failed\n"));
		return -1;
	}

	XML_SetElementHandler( parser, xmlStartFunc, xmlEndFunc);
//	XML_SetUserData( parser,g_sms_rand);
	XML_SetCharacterDataHandler( parser, xmlCharParseFunc);

	memset(buffer,0,sizeof(buffer));
	fp = fopen("./1.xml","r");
	if( fp == NULL)
	{
		DAL_ERROR(("fopen failed\n"));
		XML_ParserFree(parser);
		return -1;
	}
	fseek(fp, 0, SEEK_END); 
	file_length = ftell(fp);	
	fseek(fp, 0, SEEK_SET);
	if( file_length == 0)
	{
		DAL_ERROR(("file is empty\n"));
		XML_ParserFree(parser);
		return -1;
	}
	
	for(;;)
	{
		memset(buffer,0,sizeof(buffer));
		read_size = fread( buffer, 1, sizeof(buffer), fp);
		
		//for( i = 0; i < read_size; i++)
		//	printf(" %u  0x%x\n", i, buffer[i]);
		
		if( XML_Parse( parser, buffer, (XML_Size)read_size, 0) == XML_STATUS_ERROR)
		{
			DAL_ERROR(("XML_Parse ERROR!!!  %s at line %d\n",XML_ErrorString(XML_GetErrorCode(parser)),(int)XML_GetCurrentLineNumber(parser)));
			iret =-1;
			break;
		}
		if(feof(fp))
			break;
	}	
	XML_ParserFree(parser);
	fclose(fp);
	
	return iret;
}


char* test_asx[]= {
	"http://cybermedia.mmu.edu.my/ext/ikimfm.asx",		// "mms: mms://ucitv2.uthm.edu.my/radio2"
	"http://cybermedia.mmu.edu.my/ext/rmns.asx",		// "mms: mms://nyeng.mmu.edu.my/rmns"

	"http://www.chmbc.co.kr/vod/chmbcfm.asx",		// "mms: mms://nyeng.mmu.edu.my/rmns"
	"http://www.chmbc.co.kr/vod/chmbcamx.asx",		// "mms: mms://222.113.53.4/chmbcfm"		// 解析有问题

	"http://www.fairchildradio.com/Archive/am1430.asx",	// "mms: mms://216.18.70.242/AM1430"
	"http://www.am1470.com/liveam.asx",					// "mms: mms://216.18.70.242/AM1470N"
	"http://www.am1470.com/livefm.asx",				// "mms: mms://216.18.70.242/FM961N"
};

int g_asx_count = sizeof(test_asx);

int testParseAsx( void)
{
	httpResult_t		result;
	char				mms[256];
	int					len = 0;
	int					index = 0;
	
#if 0
	{/*read asx respone from  1.xml */
		FILE* fp = fopen("1.xml", "r");
		result.size = fread(result.data,1,512,fp);
		printf("result.size: %d\n", result.size);
		fclose(fp);
	}
#else

	//httpRequest("http://www.chmbc.co.kr/vod/chmbcfm.asx",10,&result);
	//httpRequest("http://cybermedia.mmu.edu.my/ext/ikimfm.asx",10,&result);
	//httpRequest(test_asx[index],10,&result);

	



#endif

	
	for( index = 0; index < g_asx_count; index++)
	{
		printf("\n\n_____________________________________________________________________________________\n");
		printf("parse %s \n", test_asx[index]);
		printf("_____________________________________________________________________________________\n");
		
		httpRequest(test_asx[index],10,&result);
		{
			int i = 0;
			printf("result.size: %u\n", result.size);
			for( i = 0; i< result.size; i++){
				printf("%c ",result.data[i]);
			}
			printf("\n");
		}
		parseAsx(result.data, result.size, mms, &len);
		printf("mms: %s \n" ,mms);
	}



	
	//printf("result: %s\n", result.data);
	
	//parseAsx(result.data, result.size, mms, &len);

	//parseFile();

	
	
	return 0;
}

#endif













#ifdef MMS
int get_actual_mms_url(char* mms_url,int url_len)
{
	char*			p = NULL;
	char*			q = NULL;
	char			request_url[128];
	char			actual_url[128];
	httpResult_t	result;
	int				iret = 0;
	
	assert(mms_url);

	p = strrchr(mms_url,'.');
	q = strchr(mms_url,':');
	if( NULL == p || NULL == q){
		SYS_DEBUG(SD_ERROR,MODULE_DAL,"mms_url: %s\n", actual_url);
		return 0;
	}

	if( 0 != strcasecmp(p+1,"asx")){
		SYS_DEBUG(SD_ERROR,MODULE_DAL,"not asx link\n", actual_url);
		return 0;
	}
	
	memset(request_url,0,sizeof(request_url));
	memset(actual_url,0,sizeof(actual_url));
	memset(&result,0,sizeof(httpResult_t));
	
	strcat(request_url,"http:");
	strcat(request_url,q+1);
	SYS_DEBUG(SD_DEBUG,MODULE_DAL,"request_url: %s\n",result.data);
	iret = httpRequest(request_url,10, &result);
	if( iret != 0){
		SYS_DEBUG(SD_ERROR,MODULE_DAL,"httpRequest failed\n");
		return -1;
	}
	parseAsx(result.data, result.size, actual_url, sizeof(actual_url));
	if( '\0' == actual_url[0]){
		SYS_DEBUG(SD_ERROR,MODULE_DAL,"parseAsx failed\n");
		return -1;
	}
	SYS_DEBUG(SD_DEBUG,MODULE_DAL,"actual_url: %s\n", actual_url);
	memset(mms_url,0,url_len);
	strcpy( mms_url, actual_url);
	SYS_DEBUG(SD_DEBUG,MODULE_DAL,"mms_url: %s\n", actual_url);

	return 0;
}
#endif
static char* string_convert(const char* string)
{
	int			i = 0;
	int			j = 0;
	
#ifdef EXPAT_DEBUG_RECOVER
	memset(convert_temp,0,sizeof(convert_temp));
	if( string == NULL)
		return convert_temp;
	
	while( string[i] != '\0')
	{
		convert_temp[j] = string[i];
		j++;
		i += 2;
	}
	convert_temp[j]='\0';
	return convert_temp;
#else
	return string;
#endif	
}


