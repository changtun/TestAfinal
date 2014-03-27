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
#include "anpdebug.h"

	
/*******************************************************/
/*               Private Defines and Macros			   */
/*******************************************************/

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






/******************************************************************************
* Function : VISI2C_Write
* parameters :	
*		addr : (IN) module's i2c address
*		actlen : (IN/OUT) the actual length writes to rf module
* Return :	
*		(nothing)		
*		true : FAIL
*		false : OK
* Description :
*		This function is a i2c write function of I2C module
*
* Author : LSZ	2006/01/24
******************************************************************************/


bool_t IsSpace(int ch)
{
	return ch == ' ' || ch == 9;
}

bool_t IsAlpha(int ch)
{
	return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool_t IsDigit(int ch)
{
	return ch >= '0' && ch <= '9';
}

int Hex(int ch)
{
	if (IsDigit(ch))
		return ch - '0';
	if (ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;
	if (ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;
	return  - 1;
}

void tcscpy_s(tchar_t *Out, size_t OutLen, const tchar_t *In)
{
	if (OutLen > 0)
	{
		size_t n = min(tcslen(In), OutLen - 1);
		memcpy(Out, In, n *sizeof(tchar_t));
		Out[n] = 0;
	}
}

void tcscat_s(tchar_t *Out, size_t OutLen, const tchar_t *In)
{
  if (OutLen > 0)
  {
    size_t n = tcslen(Out);
    tcscpy_s(Out + n, OutLen - n, In);
  }
}

void vstprintf_s(tchar_t *Out, size_t OutLen, const tchar_t *Mask, va_list Arg)
{
  int n, vs, Width;
  unsigned int v, w, q, w0;
  bool_t ZeroFill;
  bool_t Unsigned;
  bool_t Sign;
  bool_t AlignLeft;
  const tchar_t *In;

  while (OutLen > 1 &&  *Mask)
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

        AlignLeft =  *Mask == '-';
        if (AlignLeft)
          ++Mask;

        ZeroFill =  *Mask == '0';
        if (ZeroFill)
          ++Mask;

        Width =  - 1;
        if (IsDigit(*Mask))
        {
          Width = 0;
          for (; IsDigit(*Mask); ++Mask)
            Width = Width * 10+(*Mask - '0');
        }
        if (*Mask == '*')
        {
          ++Mask;
          Width = va_arg(Arg, int);
        }

        Unsigned =  *Mask == 'u';
        if (Unsigned)
          ++Mask;

        switch (*Mask)
        {
        case 'd':
        case 'i':
        case 'X':
        case 'x':
        case 'D':
          vs = va_arg(Arg, int);

          if (*Mask == 'x' ||  *Mask == 'X')
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

          Sign = vs < 0 && !Unsigned;
          if (Sign)
          {
            vs =  - vs;
            --Width;
          }

          w0 = 1;
          for (n = 1; n < Width; ++n)
            w0 *= q;

          v = vs;
          while (v < w && w > w0)
            w /= q;

          while (w > 0)
          {
            unsigned int i = v / w;
            v -= i * w;
            if (OutLen > 1 && Sign && (w == 1 || ZeroFill || i > 0))
            {
              *(Out++) = '-';
              --OutLen;
              Sign = 0;
            }
            if (OutLen > 1)
            {
              if (i == 0 && !ZeroFill && w != 1)
                i = ' ';
              else
              {
                ZeroFill = 1;
                if (i >= 10)
                {
                  if (*Mask == 'X')
                    i += 'A' - 10;
                  else
                    i += 'a' - 10;
                }
                else
                  i += '0';
              }
              *(Out++) = (tchar_t)i;
              --OutLen;
            }
            w /= q;
          }

          break;

        case 'c':
          *(Out++) = (tchar_t)va_arg(Arg, int);
          --OutLen;
          break;

        case 's':
          In = va_arg(Arg, const tchar_t*);
          n = min(tcslen(In), OutLen - 1);
          Width -= n;
          if (!AlignLeft)
          while (--Width >= 0 && OutLen > 1)
          {
            *Out++ = ' ';
            --OutLen;
          }
          memcpy(Out, In, n *sizeof(tchar_t));
          Out += n;
          OutLen -= n;
          while (--Width >= 0 && OutLen > 1)
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

  if (OutLen > 0)
    *Out = 0;
}

int stscanf(const tchar_t *In, const tchar_t *Mask, ...)
{
  va_list Arg;
  int n = 0;
  int Sign;
  int v;
  int Width;
  const tchar_t *In0;

  va_start(Arg, Mask);
  while (In &&  *In &&  *Mask)
  {
    switch (*Mask)
    {
      case '%':
        ++Mask;

        Width =  - 1;
        if (IsDigit(*Mask))
        {
          Width = 0;
          for (; IsDigit(*Mask); ++Mask)
            Width = Width * 10+(*Mask - '0');
        }

        switch (*Mask)
        {
        case 'X':
        case 'x':

          for (; IsSpace(*In); ++In)
            ;
          v = 0;
          Sign =  *In == '-';
          In0 = In;
          if (Sign)
          {
            ++In;
            --Width;
          }
          for (; Width != 0 &&  *In; ++In, --Width)
          {
            int h = Hex(*In);
            if (h < 0)
              break;
            v = v * 16+h;
          }
          if (Sign)
            v =  - v;
          if (In != In0)
          {
            *va_arg(Arg, int*) = v;
            ++n;
          }
          else
            In = NULL;
          break;

        case 'd':
        case 'i':

          for (; IsSpace(*In); ++In)
            ;
          v = 0;
          Sign =  *In == '-';
          In0 = In;
          if (Sign)
            ++In;
          for (; Width != 0 && IsDigit(*In); ++In, --Width)
            v = v * 10+(*In - '0');
          if (Sign)
            v =  - v;
          if (In != In0)
          {
            *va_arg(Arg, int*) = v;
            ++n;
          }
          else
            In = NULL;
          break;

        case 'o':

          for (; IsSpace(*In); ++In)
            ;
          v = 0;
          Sign =  *In == '-';
          In0 = In;
          if (Sign)
            ++In;
          for (; Width != 0 &&  *In; ++In, --Width)
          {
            if (*In >= '0' &&  *In <= '7')
              v = v * 8+(*In - '0');
            else
              break;
          }
          if (Sign)
            v =  - v;
          if (In != In0)
          {
            *va_arg(Arg, int*) = v;
            ++n;
          }
          else
            In = NULL;
          break;
        }
        break;
      case 9:
      case ' ':
        for (; IsSpace(*In); ++In)
          ;
        break;
      default:
        if (*Mask ==  *In)
          ++In;
        else
        {
          In = NULL;
          n =  - 1;
        }
    }
    ++Mask;
  }

  va_end(Arg);
  return n;
}

void DebugMessage(const tchar_t *Msg, ...)
{
  va_list Args;
  tchar_t s[1024] =
  {
    0
  };
  va_start(Args, Msg);
  vstprintf_s(s + tcslen(s), TSIZEOF(s) - tcslen(s), Msg, Args);
  va_end(Args);
  tcscat_s(s, TSIZEOF(s), T("\n"));

  LOGI(s);
}


