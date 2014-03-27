/*****************************************************************************
 *
 * This program is free software ; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * $Id: platform_win32.c 622 2006-01-31 19:02:53Z picard $
 *
 * The Core Pocket Media Player
 * Copyright (c) 2004-2005 Gabor Kovacs
 *
 ****************************************************************************/

#include "logit.h"     /* port dependant system files */

#if defined(TARGET_ANDROID)

#include <android/log.h>
#define LOG_TAG "DVB-jni"
#define LOGI(a)  __android_log_write(ANDROID_LOG_INFO,LOG_TAG,a)

#endif

void DebugMessage(const char* Msg,...)
{
	va_list Args;
	char s[1024];

	s[0]=0;


	va_start(Args,Msg);
	vstprintf_s(s+strlen(s),TSIZEOF(s)-strlen(s), Msg, Args);
	va_end(Args);
	tcscat_s(s,TSIZEOF(s),T("\n"));
#if defined(TARGET_ANDROID)
	LOGI(s);
#endif
}

int IsSpace(int ch) { return ch==' ' || ch==9; }
int IsAlpha(int ch) { return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'); }
int IsDigit(int ch) { return ch>='0' && ch<='9'; }

void tcscpy_s(char* Out,size_t OutLen,const char* In)
{
	if (OutLen>0)
	{
		size_t n = min(strlen(In),OutLen-1);
		memcpy(Out,In,n*sizeof(char));
		Out[n] = 0;
	}
}

void tcscat_s(char* Out,size_t OutLen,const char* In)
{
	if (OutLen>0)
	{
		size_t n = strlen(Out);
		tcscpy_s(Out+n,OutLen-n,In);
	}
}


void vstprintf_s(char* Out,size_t OutLen,const char* Mask,va_list Arg)
{
	int n,vs,Width;
	unsigned int v,w,q,w0;
	int ZeroFill;
	int Unsigned;
	int Sign;
	int AlignLeft;
	const char *In;

	while (OutLen>1 && *Mask)
	{
		switch (*Mask)
		{
		case '%':
			++Mask;

			if (*Mask == '%')
			{
				*(Out++) = *(Mask++);
				--OutLen;
				break;
			}

			AlignLeft = *Mask=='-';
			if (AlignLeft)
				++Mask;

			ZeroFill = *Mask=='0';
			if (ZeroFill) 
				++Mask;

			Width = -1;
			if (IsDigit(*Mask))
			{
				Width = 0;
				for (;IsDigit(*Mask);++Mask)
					Width = Width*10 + (*Mask-'0');
			}
			if (*Mask == '*')
			{
				++Mask;
				Width = va_arg(Arg,int);
			}

			Unsigned = *Mask=='u';
			if (Unsigned)
				++Mask;

			switch (*Mask)
			{
			case 'd':
			case 'i':
			case 'X':
			case 'x':
				vs = va_arg(Arg,int);

				if (*Mask=='x' || *Mask=='X')
				{
					Unsigned = 1;
					q = 16;
					w = 0x10000000;
				}
				else
				{
					q = 10;
					w = 1000000000;
				}

				Sign = vs<0 && !Unsigned;
				if (Sign)
				{
					vs=-vs;
					--Width;
				}

				w0 = 1;
				for (n=1;n<Width;++n)
					w0 *= q;

				v = vs;
				while (v<w && w>w0)
					w/=q;

				while (w>0)
				{
					unsigned int i = v/w;
					v-=i*w;
					if (OutLen>1 && Sign && (w==1 || ZeroFill || i>0))
					{
						*(Out++) = '-';
						--OutLen;
						Sign = 0;
					}
					if (OutLen>1)
					{
						if (i==0 && !ZeroFill && w!=1)
							i = ' ';
						else
						{
							ZeroFill = 1;
							if (i>=10)
							{
								if (*Mask == 'X')
									i += 'A'-10;
								else
									i += 'a'-10;
							}
							else
								i+='0';
						}
						*(Out++) = (char)i;
						--OutLen;
					}
					w/=q;
				}

				break;

			case 'c':
				*(Out++) = (char)va_arg(Arg,int);
				--OutLen;
				break;

			case 's':
				In = va_arg(Arg,const char*);
				n = min(strlen(In),OutLen-1);
				Width -= n;
				if (!AlignLeft)
					while (--Width>=0 && OutLen>1)
					{
						*Out++ = ' ';
						--OutLen;
					}
					memcpy(Out,In,n*sizeof(char));
					Out += n;
					OutLen -= n;
					while (--Width>=0 && OutLen>1)
					{
						*Out++ = ' ';
						--OutLen;
					}
					break;
			}

			++Mask;
			break;

		default:
			*(Out++) = *(Mask++);
			--OutLen;
			break;
		}
	}

	if (OutLen>0)
		*Out=0;
}

void stcatprintf_s(char* Out,size_t OutLen,const char* Mask, ...)
{
	size_t n = strlen(Out);
	va_list Arg;
	va_start(Arg, Mask);
	vstprintf_s(Out+n,OutLen-n,Mask,Arg);
	va_end(Arg);
}

void DebugBinary(const char* Msg,const void* Data,int Length)
{
	const unsigned char* p = (const unsigned char*)Data;
	int i;
	char s[1024];
	while (Length>0)
	{
		tcscpy_s(s,TSIZEOF(s),Msg);
		for (i=0;Length>0 && i<1024;++i,--Length,++p)
			stcatprintf_s(s,TSIZEOF(s),T(" %02x"),*p);
		DebugMessage(s);
	}
}
int DebugMask() 
{
	return DEBUG_TEST;
}
