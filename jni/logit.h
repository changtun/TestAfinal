#ifndef _LOGIT_H
#define _LOGIT_H
/* LOG */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>


#ifdef __cplusplus
extern "C" {
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define TSIZEOF(name)	(sizeof(name)/sizeof(char))
#define T(a) a

int  DebugMask();
void DebugMessage(const char *,...);
void DebugBinary(const char* Msg,const void* Data,int Length);

void vstprintf_s(char* Out,size_t OutLen,const char* Mask,va_list Arg);
void tcscpy_s(char* Out,size_t OutLen,const char* In);
void tcscat_s(char* Out,size_t OutLen,const char* In);
void stcatprintf_s(char* Out,size_t OutLen,const char* Mask, ...);
void DebugBinary(const char* Msg,const void* Data,int Length);

#define DEBUG_TEST				0x01

#if defined(USE_LOGIT)
#define DEBUG_BIN(m,x,p,n) if (DebugMask() & (m)) DebugBinary(x,p,n)
#define DEBUG_MSG(m,x) if (DebugMask() & (m)) DebugMessage(x)
#define DEBUG_MSG1(m,x,a) if (DebugMask() & (m)) DebugMessage(x,a)
#define DEBUG_MSG2(m,x,a,b) if (DebugMask() & (m)) DebugMessage(x,a,b)
#define DEBUG_MSG3(m,x,a,b,c) if (DebugMask() & (m)) DebugMessage(x,a,b,c)
#define DEBUG_MSG4(m,x,a,b,c,d) if (DebugMask() & (m)) DebugMessage(x,a,b,c,d)
#define DEBUG_MSG5(m,x,a,b,c,d,e) if (DebugMask() & (m)) DebugMessage(x,a,b,c,d,e)
#define DEBUG_MSG6(m,x,a,b,c,d,e,f) if (DebugMask() & (m)) DebugMessage(x,a,b,c,d,e,f)
#define DEBUG_MSG7(m,x,a,b,c,d,e,f,g) if (DebugMask() & (m)) DebugMessage(x,a,b,c,d,e,f,g)
#else
#define DEBUG_BIN(m,x,p,n)
#define DEBUG_MSG(m,x)
#define DEBUG_MSG1(m,x,a)
#define DEBUG_MSG2(m,x,a,b)
#define DEBUG_MSG3(m,x,a,b,c)
#define DEBUG_MSG4(m,x,a,b,c,d)
#define DEBUG_MSG5(m,x,a,b,c,d,e)
#define DEBUG_MSG6(m,x,a,b,c,d,e,f)
#define DEBUG_MSG7(m,x,a,b,c,d,e,f,g)
#endif
#ifdef __cplusplus
}
#endif

#endif
