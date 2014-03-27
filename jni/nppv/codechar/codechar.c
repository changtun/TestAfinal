

#include "iconv.h"

#define CHARACTER_GBK  0
#define CHARACTER_BIG5 1
#define CHARACTER_UNICODE 2
#define CHARACTER_UTF8 3

#define UTF8	"UTF-8"
#define UTF16	"UTF-16"
#define GBK  	"GBK"
#define BIG5	"BIG-5"
#define GB2312	"GB2312"
#define UNICODE "UNICODE-1-1"

#define CHARACTER_TYPE_IEC10646 	0x11
#define CHARACTER_TYPE_KSX		0x12
#define CHARACTER_TYPE_GB2312 	0x13
#define	CHARACTER_TYPE_BIG5		0x14
#define	CHARACTER_TYPE_UTF8		0x15

static int default_charset = 0;

static int _code_convert( const char *from_charset, const char *to_charset, char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
    iconv_t cd;
    int ret = 0;
    char **pin = &inbuf;
    char **pout = &outbuf;

    cd = iconv_open(to_charset, from_charset);
    if (cd == (iconv_t) - 1)
    {
        return -1;
    }
    memset(outbuf, 0, outlen);
    ret = (size_t)iconv(cd, pin, &inlen, pout, &outlen);
    if( -1 == ret )
    {
        iconv_close(cd);
        return -1;
    }
    iconv_close(cd);
    return outlen;
}

int Utf16ToUtf8(char *inbuf, int inlen, char *outbuf, int outlen)
{
    if( (NULL == inbuf) || (NULL == outbuf) || (0 == inlen) || (outlen < inlen) )
    {
        return -1;
    }
    return _code_convert(UTF16, UTF8, inbuf, (size_t)inlen, outbuf, (size_t)outlen);
}

int Big5ToUtf8(char *inbuf, int inlen, char *outbuf, int outlen)
{
    if( (NULL == inbuf) || (NULL == outbuf) || (0 == inlen) || (outlen < inlen) )
    {
        return -1;
    }
    return _code_convert(BIG5, UTF8, inbuf, (size_t)inlen, outbuf, (size_t)outlen);
}
int Gb2312ToUtf8(char *inbuf, int inlen, char *outbuf, int outlen)
{
    if( (NULL == inbuf) || (NULL == outbuf) || (0 == inlen) || (outlen < inlen) )
    {
        return -1;
    }
    return _code_convert(GB2312, UTF8, inbuf, (size_t)inlen, outbuf, (size_t)outlen);
}

int GbkToUtf8(char *inbuf, int inlen, char *outbuf, int outlen)
{
    if( (NULL == inbuf) || (NULL == outbuf) || (0 == inlen) || (outlen < inlen) )
    {
        return -1;
    }
    return _code_convert(GBK, UTF8, inbuf, (size_t)inlen, outbuf, (size_t)outlen);
}

int UnicodeToUtf8(char *inbuf, int inlen, char *outbuf, int outlen)
{
    if( (NULL == inbuf) || (NULL == outbuf) || (0 == inlen) || (outlen < inlen) )
    {
        return -1;
    }

    return _code_convert(UNICODE, UTF8, inbuf, (size_t)inlen, outbuf, (size_t)outlen);
}

int Code_Init(void)
{
    default_charset = CHARACTER_GBK;
    return 0;
}

int Code_Set_Default(int set_val)
{
    default_charset = set_val;
    return 0;
}


char *Code_ToChar(int CodeType, char *str, int strLen )
{

    int iInLen = 0;
    int iOutLen = 0;
    int iRet = 0;
    char *pInBuf = NULL;
    char *pOutBuf = NULL;

    iInLen = strLen;
    iOutLen = iInLen * 4;

    if( NULL == str )
    {
        return NULL;
    }

    pInBuf = (char *)malloc(iInLen + 2);
    if( NULL == pInBuf )
    {
        return NULL;
    }
    memset( pInBuf, 0, iInLen + 2);
    memcpy( pInBuf, str, iInLen );


    pOutBuf = (char *)malloc(iOutLen);
    if( NULL == pOutBuf )
    {
        return NULL;
    }


    memset( pOutBuf, 0, iOutLen);
    switch( CodeType )
    {
    case CHARACTER_TYPE_IEC10646:
    {

        if( CHARACTER_TYPE_IEC10646 == pInBuf[0] )
        {

            iRet = UnicodeToUtf8( pInBuf + 1, iInLen - 1, pOutBuf, iOutLen );
        }
        else
        {
            iRet = UnicodeToUtf8( pInBuf, iInLen, pOutBuf, iOutLen);
        }

        if( -1 == iRet )
        {
            free(pOutBuf);
            return NULL;
        }

    }
    break;
    case CHARACTER_TYPE_GB2312:
    {
        if( CHARACTER_TYPE_GB2312 == pInBuf[0] )
        {
            iRet = GbkToUtf8( pInBuf + 1, iInLen - 1, pOutBuf, iOutLen );
        }
        else
        {
            iRet = GbkToUtf8( pInBuf, iInLen, pOutBuf, iOutLen );
        }
        if( -1 == iRet )
        {
            free(pOutBuf);
            return NULL;
        }
    }
    break;
    case CHARACTER_TYPE_BIG5:
    {
        if( CHARACTER_TYPE_BIG5 == pInBuf[0] )
        {
            iRet = Big5ToUtf8( pInBuf + 1, iInLen - 1, pOutBuf, iOutLen );
        }
        else
        {
            iRet = Big5ToUtf8( pInBuf, iInLen, pOutBuf, iOutLen );
        }
        if( -1 == iRet )
        {
            free(pOutBuf);
            return NULL;
        }
    }
    break;
    case CHARACTER_TYPE_UTF8:
    {
        free(pOutBuf);
        return pInBuf;
    }
    break;
    default:
    {

        switch(default_charset)
        {
        case CHARACTER_GBK:
            iRet = GbkToUtf8( str, iInLen, pOutBuf, iOutLen);
            if( -1 == iRet )
            {
                free(pOutBuf);
                return NULL;
            }
            break;
        case CHARACTER_BIG5:
            iRet = Big5ToUtf8( pInBuf, iInLen, pOutBuf, iOutLen);
            if( -1 == iRet )
            {
                free(pOutBuf);
                return NULL;
            }
            break;
        case CHARACTER_UNICODE:
            iRet = UnicodeToUtf8( str, iInLen, pOutBuf, iOutLen);
            if( -1 == iRet )
            {
                free(pOutBuf);
                return NULL;
            }
            break;
        case CHARACTER_UTF8:
            free(pOutBuf);
            return pInBuf;
            break;
        default:
            iRet = GbkToUtf8( str, iInLen, pOutBuf, iOutLen);
            if( -1 == iRet )
            {
                free(pOutBuf);
                return NULL;
            }
            break;
        }
        if( -1 == iRet )
        {
            if(pOutBuf != NULL)
                free(pOutBuf);
            return NULL;
        }
    }
    break;
    }
    return pOutBuf;
}


