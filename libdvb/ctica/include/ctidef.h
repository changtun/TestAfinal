/*
 * ctidef.h
 *
 *  Created on: 2010-3-3
 *      Author: znjin
 */

#ifndef TYPE_H_
#define TYPE_H_

#include <string.h>

/*------------------------------ Constants Definitions ------------------------*/
#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#define ON     	1
#define OFF    	0
#define NOUSED	0xfe

#define VOID	void
#define CHAR	char
#define BYTE	unsigned char
#define SINT	signed char
#define	WORD	unsigned short
#define INT		signed int
#define UINT	unsigned int
#define LONG	long
#define ULONG	unsigned long

#ifndef BOOLEAN
#define BOOLEAN		CHAR
#endif

#define PBYTE		BYTE *
#define PVOID		VOID *
#define PWORD		WORD *

typedef	ULONG	CTI_OsSemaphore_t;
typedef	ULONG	CTI_MailBoxOfTask_t;

#define CARD_IN      0
#define CARD_OUT     1
#define CARD_ERROR   2

#endif /* TYPE_H_ */
