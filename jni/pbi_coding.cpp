#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nativehelper/jni.h>
#include <errno.h>
#include "iconv.h"

#include "logit.h"
#include "pbi_coding.h"
#include "jnistaple.h"
#include "file_config.h"

static int _code_convert( const char* from_charset, const char* to_charset, char* inbuf, size_t inlen, char* outbuf, size_t outlen)
{
    iconv_t cd;
	int ret = 0;
    char **pin = &inbuf;
    char **pout = &outbuf;
	
    cd = iconv_open(to_charset, from_charset);
    if (cd == (iconv_t)-1)
    {
        DEBUG_MSG1( -1, T("iconv open error!! %s."),strerror(errno) );
        return -1;
    }

    memset(outbuf,0,outlen);
	ret = (size_t)iconv(cd, pin, &inlen, pout, &outlen);
    if( -1 == ret )
    {
        //printf("%s: call iconv failed!\n", __FUNCTION__);
        DEBUG_MSG1( -1, T("iconv error!! %s."),strerror(errno) );
        iconv_close(cd);
        return -1;
    }
    iconv_close(cd);
    return outlen;
}

int Utf16ToUtf8(char* inbuf, int inlen, char* outbuf, int outlen)
{
	if( (NULL == inbuf) || (NULL == outbuf) || (0 == inlen) || (outlen < inlen) )
	{
		DEBUG_MSG( -1, T("Utf16ToUtf8 param error!!") );
		return -1;
	}
	return _code_convert(UTF16, UTF8, inbuf, (size_t)inlen, outbuf, (size_t)outlen);
}

int Big5ToUtf8(char* inbuf, int inlen, char* outbuf, int outlen)
{
	if( (NULL == inbuf) || (NULL == outbuf) || (0 == inlen) || (outlen < inlen) )
	{
		DEBUG_MSG( -1, T("Big5toUtf8 param error!!") );
		return -1;
	}
	return _code_convert(BIG5, UTF8, inbuf, (size_t)inlen, outbuf, (size_t)outlen);
}
int Gb2312ToUtf8(char* inbuf, int inlen, char* outbuf, int outlen)
{
	if( (NULL == inbuf) || (NULL == outbuf) || (0 == inlen) || (outlen < inlen) )
	{
		DEBUG_MSG( -1, T("GbkToUtf8 param error!!") );
		return -1;
	}
	return _code_convert(GB2312, UTF8, inbuf, (size_t)inlen, outbuf, (size_t)outlen);
}

int GbkToUtf8(char* inbuf, int inlen, char* outbuf, int outlen)
{
	if( (NULL == inbuf) || (NULL == outbuf) || (0 == inlen) || (outlen < inlen) )
	{
		DEBUG_MSG( -1, T("GbkToUtf8 param error!!") );
		return -1;
	}
	return _code_convert(GBK, UTF8, inbuf, (size_t)inlen, outbuf, (size_t)outlen);
}

int UnicodeToUtf8(char* inbuf, int inlen, char* outbuf, int outlen)
{
	if( (NULL == inbuf) || (NULL == outbuf) || (0 == inlen) || (outlen < inlen) )
	{
		DEBUG_MSG( -1, T("UnicodeToUtf8 param error!!") );
		return -1;
	}
	return _code_convert(UNICODE, UTF8, inbuf, (size_t)inlen, outbuf, (size_t)outlen);
}

jstring CodingToJstring( JNIEnv *env, int CodeType, char *str, int strLen )
{
	jstring jName = NULL;

	int iInLen = 0;
	int iOutLen = 0;
	int iRet = 0;
	char *pInBuf = NULL;
	char *pOutBuf = NULL;

	iInLen = strLen;
	iOutLen = iInLen * 4;

	if( NULL == str )
	{
		return jName;
	}
	pInBuf = (char*)malloc(iInLen+2);
	if( NULL == pInBuf )
	{
		return jName;
	}
	memset( pInBuf, 0, iInLen + 2);
	memcpy( pInBuf, str, iInLen );
	
	pOutBuf = (char*)malloc(iOutLen);
	if( NULL == pOutBuf )
	{
		return jName;
	}
	
	memset( pOutBuf, 0, iOutLen);
	//DEBUG_MSG1(-1, "zxguan coding type 0x%x....", CodeType);
	switch( CodeType )
	{
		case CHARACTER_TYPE_IEC10646:
		{

			if( CHARACTER_TYPE_IEC10646 == pInBuf[0] )
			{
				
				iRet = UnicodeToUtf8( pInBuf+1, iInLen-1, pOutBuf, iOutLen );
			}
			else
			{
				iRet = UnicodeToUtf8( pInBuf, iInLen, pOutBuf, iOutLen);
			}
			if( -1 == iRet )
			{
				free(pOutBuf);
				free(pInBuf);
				return jName;
			}
			
		}break;
		case CHARACTER_TYPE_GB2312:
		{
			if( CHARACTER_TYPE_GB2312 == pInBuf[0] )
			{
				iRet = GbkToUtf8( pInBuf+1, iInLen-1, pOutBuf, iOutLen );
			}
			else
			{
				iRet = GbkToUtf8( pInBuf, iInLen, pOutBuf, iOutLen );
			}
			if( -1 == iRet )
			{

				free(pOutBuf);
				free(pInBuf);
				return jName;
			}
		}break;
		case CHARACTER_TYPE_BIG5:
		{
			if( CHARACTER_TYPE_BIG5 == pInBuf[0] )
			{
				iRet = Big5ToUtf8( pInBuf+1, iInLen-1, pOutBuf, iOutLen );
			}
			else
			{
				iRet = Big5ToUtf8( pInBuf, iInLen, pOutBuf, iOutLen );
			}
			if( -1 == iRet )
			{
				free(pOutBuf);
				free(pInBuf);
				return jName;
			}
		}break;
		case CHARACTER_TYPE_UTF8:
		{
			jName = stoJstring_UTF8( env, str, iInLen );
			free(pOutBuf);
			free(pInBuf);
			return jName;
		}break;
		default:
		{
			eDEFAULT_CHARACTER_ENCODE eDefault;
			eDefault = GetDefaultCharacterEncode();
			switch(eDefault)
			{
				case CHARACTER_GBK:
					iRet = GbkToUtf8( str, iInLen, pOutBuf, iOutLen);
					break;
    			case CHARACTER_BIG5:
					iRet = Big5ToUtf8( pInBuf, iInLen, pOutBuf, iOutLen);
					break;
    			case CHARACTER_UNICODE:
					iRet = UnicodeToUtf8( str, iInLen, pOutBuf, iOutLen);
					break;
    			case CHARACTER_UTF8:
					memcpy( pOutBuf,pInBuf,iInLen);
					break;
				default:
					iRet = GbkToUtf8( str, iInLen, pOutBuf, iOutLen);
					break;
			}
			//iRet = UnicodeToUtf8( str, iInLen, pOutBuf, iOutLen);
			//iRet = Gb2312ToUtf8( str, iInLen, pOutBuf, iOutLen);
			//iRet = GbkToUtf8( str, iInLen, pOutBuf, iOutLen);
			//iRet = Utf16ToUtf8( pInBuf, iInLen, pOutBuf, iOutLen);
			//iRet = Big5ToUtf8( pInBuf, iInLen, pOutBuf, iOutLen);
			if( -1 == iRet )
			{
				free(pOutBuf);
				free(pInBuf);
				return jName;
			}
		}break;
	}
#if 0
	DEBUG_MSG1(-1,"zxguan utf8 Len %d....",iRet);
	int bb = 0;
	for(; bb< iRet; bb++)
	{
		DEBUG_MSG1(-1,"zxguan 0x%x  ",pOutBuf[bb]);
	}
#endif	
	jName = stoJstring_UTF8( env, pOutBuf, strlen(pOutBuf) );

	free(pOutBuf);
	free(pInBuf);

	return jName;
}