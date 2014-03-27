/*******************************************************************

  Source file name : ctios.h

  Description:
	OS support APIs for all applications of integrating CTI's CA.

  COPYRIGHT (C) Compunicate Technologies, Inc. 2003-2005

*********************************************************************/

#include "ctios.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "osapi.h" 
#include <stdarg.h>
#include "pbitrace.h"
#include <android/log.h>
#include<ctiapi.h>

#define MAX_CAS_TASK_NUM     (25)
#define MAX_CAS_MSG_NUM		 (400)
#define MAX_CAS_SEM_NUM		 (30)

#define DEBUGPF pbiinfo


typedef struct cas_task_msg{
	U32 task_id;	
	U32 msgque_id;
	BOOL use_flag;
}cas_task_msg_t;


typedef struct cas_semphore_s{
	U32  sempid;
	BOOL use_flag;
}cas_semphore_t;


static cas_task_msg_t g_ctica_task_info[MAX_CAS_TASK_NUM] ;
static U32    msg_protect_sem =0xffffffff;
static cas_semphore_t g_ctica_sem_info[MAX_CAS_SEM_NUM];
static U8 sem_num = 0;
static U8 task_num = 0;


/*******************************************************/
/*               Private Function prototypes		                          */
/*******************************************************/

/*******************************************************/
/*               Functions							                   */
/*******************************************************/

/******************************************************************************
* Function :CTICA_OS_Init
* Parameters : 
*			void
* Return :
*			void
* Description :
*                   The function is to init the ctica os
* Author : 
*                   zwqiu     2013-04-12
* ------------------------------
* Modification History:       
*                     Initalization
* --------------------
*****************************************************************************/
void CTICA_OS_Init(void)
{
	int i;
	for( i=0; i<MAX_CAS_TASK_NUM; i++)
	{
		g_ctica_task_info[i].task_id =0;
		g_ctica_task_info[i].msgque_id =0xffffffff;
		g_ctica_task_info[i].use_flag =FALSE;
		
	}
	for( i=0; i<MAX_CAS_SEM_NUM; i++)
	{
		g_ctica_sem_info[i].sempid=0xffffffff;
		g_ctica_sem_info[i].use_flag =FALSE;
		
	}

	
	//sprintf(sem_name,"%s","sem_protect");
	if(OS_SemCreate(&msg_protect_sem,"sem_protect",1,0) !=0)
	{
		pbierror("CTICA_OS_Init---->error \n");
		//return 0xffffffff;
	}
	//memset(g_ctica_task_info,0,sizeof(cas_task_msg_t)*MAX_CAS_TASK_NUM);
}

/*去初始化ca部分包括信号量、task、内存、消息队列*/
void CTICA_OS_UnInit(void)
{
	int i;
	S32  ret =0;
	
	DEBUGPF("-----------CTICA_OS_UnInit  %s    %d \n",__FUNCTION__,__LINE__);
	sem_num =0;
	task_num =0;
		
	for( i=0; i<MAX_CAS_TASK_NUM; i++)
	{

		if(g_ctica_task_info[i].task_id != 0)	
		{
			DEBUGPF("-----------CTICA_OS_UnInit   %d    taskid %x \n",__LINE__,g_ctica_task_info[i].task_id);
			
			ret =OS_TaskDelete(g_ctica_task_info[i].task_id);
			if(ret !=0)
			{
				pbierror("CTICA_OS_unInit-----taskid %x \n",g_ctica_task_info[i].task_id);
			}
			g_ctica_task_info[i].task_id =0;			
			g_ctica_task_info[i].use_flag =FALSE;
		}
	}
		
	for( i=0; i<MAX_CAS_TASK_NUM; i++)
	{
	
		if(g_ctica_task_info[i].msgque_id != 0xffffffff)	
		{
			DEBUGPF("-----------CTICA_OS_UnInit   %d  msgid %x \n",__LINE__,g_ctica_task_info[i].msgque_id);
			ret =OS_MessageQueueDelete(g_ctica_task_info[i].msgque_id);
			if(ret !=0)
			{
				pbierror("CTICA_OS_unInit-----msgque_id %x \n",g_ctica_task_info[i].msgque_id);
			}

		}
		g_ctica_task_info[i].msgque_id =0xffffffff;
	}
		
		
		
	for( i=0; i<MAX_CAS_SEM_NUM; i++)
	{
		if(g_ctica_sem_info[i].use_flag == TRUE)
		{
			DEBUGPF("CTICA_OS_UnInit   line %d \n",__LINE__);
			ret =OS_SemDelete1(g_ctica_sem_info[i].sempid);
			if(ret !=0)
			{
				pbierror("CTICA_OS_unInit-----OS_SemDelete error	%x \n",g_ctica_sem_info[i].sempid);
			}			
		}
		g_ctica_sem_info[i].sempid=0xffffffff;
		g_ctica_sem_info[i].use_flag =FALSE;
	
	}

	if(msg_protect_sem!=0xffffffff)
	{
		ret =OS_SemDelete(msg_protect_sem);
		if(ret !=0)
		{
			pbierror("CTICA_OS_unInit-----OS_SemDelete error  %x \n",g_ctica_task_info[i].msgque_id);
		}
		msg_protect_sem = 0xffffffff;
	}
	DEBUGPF("-----------ctica  %s    %d   ok\n",__FUNCTION__,__LINE__);

}



PVOID CTIOS_MemoryAllocate(ULONG uSize)
{
	//DEBUGPF("******** %s  %d  size%d\n",__FUNCTION__,__LINE__,uSize);
	if(uSize > 0)  
	{
		
		return OS_MemAllocate(system_p, uSize);
	}
	//DEBUGPF("TRDRV_OS_AllocateMemory--------->ulSize %d error \n",uSize);
	return NULL;

}

VOID  CTIOS_MemoryFree(PVOID pvMemory)
{
	if(pvMemory == NULL)
	{
		pbierror("TRDRV_OS_FreeMemory-----------> param error \n");
		return;
	}  
	//DEBUGPF("******** %s  %d  \n",__FUNCTION__,__LINE__);
	 OS_MemDeallocate(system_p, pvMemory);
	// DEBUGPF("************* %s  %d   ok \n",__FUNCTION__,__LINE__);

}

CTI_MailBoxOfTask_t CTIOS_TaskCreate(VOID(*task)(PVOID arg),INT iPriority,INT iStackSize)
{
	U32 task_id;
	U8  name[20] ={0};
	U8  i;  
	U32  msg_id;
	U32  os_err;
	
	if((task ==NULL)||(iStackSize <=0))
	{
		pbierror("TRDRV_OS_CreateTask-------> param error statcksize %d \n",iStackSize);
		return 0;
	}

	sprintf(name,"%s_%d","cticastask",task_num);
	
	os_err = OS_TaskCreate(&task_id,&name,(&(*task)),NULL,NULL,iStackSize,iPriority,0);
		                 //&task_id,(U8*)(&name),(void *(* task)),NULL,NULL,ulStackSize,iPriority,0);
	if(os_err!=OS_SUCCESS)
	{
		pbierror("TRDRV_OS_CreateTask--->create task error \n");
		return 0;
	}
	task_num++;

	for(i=0; i<MAX_CAS_TASK_NUM; i++)
	{
		if(g_ctica_task_info[i].use_flag ==FALSE)
		{
			break;
		}
	}
	if(i == MAX_CAS_TASK_NUM)
	{
		pbierror("TRDRV_OS_CreateTask---error ---task full\n");
		return task_id;		
	}

	sprintf(name,"citcasmsg_%d",task_num-1);
	
	os_err = OS_QueueCreate(&msg_id, name,MAX_CAS_MSG_NUM, sizeof(CTI_OsMessage_t), 0);
	if(OS_SUCCESS != os_err)
	{
		pbierror("TRDRV_OS_CreateTask, create message queue failed! Errcode = %d.\n", os_err);
		g_ctica_task_info[i].use_flag =TRUE;
		g_ctica_task_info[i].task_id =task_id;
		return task_id; 
	}
	
	DEBUGPF("&&&&&&&&&&&&&&& TRDRV_OS_CreateTaskTRDRV_OS_CreateTaskTRDRV_OS_CreateTaskTRDRV_OS_CreateTask --------------%d\n tasdid %x\n",iPriority,task_id);

	g_ctica_task_info[i].use_flag =TRUE;
	g_ctica_task_info[i].task_id =task_id;
	g_ctica_task_info[i].msgque_id = msg_id;
	
	
	return (CTI_MailBoxOfTask_t)task_id;
}
CTI_OsSemaphore_t CTIOS_CreateSemaphore(ULONG uInitialValue)
{
	U32  sem_id = 0;
	char sem_name[20] ={0};
	U8   i;
	
	for( i=0; i<MAX_CAS_SEM_NUM; i++)
	{
		if(g_ctica_sem_info[i].use_flag ==FALSE)
		{
			break;
		}
	}
	if(i>=MAX_CAS_SEM_NUM)
	{
		pbierror("TRDRV_OS_CreateSemaphore---->error 111\n");
		return 0;

	}
	

	sprintf(sem_name,"sem_%d",sem_num);
	if(OS_SemCreate(&sem_id,sem_name,uInitialValue,0) !=0)
	{
		pbierror("TRDRV_OS_CreateSemaphore---->error \n");
		return 0;
	}

	g_ctica_sem_info[i].sempid =sem_id;
	g_ctica_sem_info[i].use_flag =TRUE;
	
	DEBUGPF("---------------TRDRV_OS_CreateSemaphore ---sid [%s|0x%x]initValue[%d]\n",sem_name, sem_id, uInitialValue);
	sem_num++;

	return (CTI_OsSemaphore_t)sem_id;
	
}
VOID CTIOS_SignalSemaphore(CTI_OsSemaphore_t uSemaphore)
{
	

	U8   i;
	//DEBUGPF("\n\nCTIOS_SignalSemaphore start id-----------------[0x%x]\n",uSemaphore);
	
	for( i=0; i<MAX_CAS_SEM_NUM; i++)
	{
		if((g_ctica_sem_info[i].use_flag ==TRUE)&&(uSemaphore == g_ctica_sem_info[i].sempid))
		{
			break;
		}
	}

	if(i>=MAX_CAS_SEM_NUM)
	{
		pbierror("TRDRV_OS_SignalSemaphore--->error  line %d\n",__LINE__);
		return;

	}
	
	/*if(ulSemaphore == 0)
	{
		pbierror("TRDRV_OS_SignalSemaphore--->error\n");
		return;
	}*/
	OS_SemSignal(uSemaphore);
	//DEBUGPF("CTIOS_SignalSemaphore OK!!!!-----------------[0x%x]\n\n",uSemaphore);
}
VOID CTIOS_WaitSemaphore(CTI_OsSemaphore_t uSemaphore)
{
	//DEBUGPF("\n\nCTIOS_WaitSemaphore  start ---------------id[0x%x]\n",uSemaphore);

	U8	 i;
		
	for( i=0; i<MAX_CAS_SEM_NUM; i++)
	{
		if((g_ctica_sem_info[i].use_flag ==TRUE)&&(uSemaphore == g_ctica_sem_info[i].sempid))
		{
			break;
		}
	}

	if(i>=MAX_CAS_SEM_NUM)
	{
		pbierror("TRDRV_OS_WaitSemaphore--->error  line %d\n",__LINE__);
		return;

	}

	
	/*if((ulSemaphore == 0)||(ulSemaphore ==0xffffffff))
	{
		pbierror("TRDRV_OS_WaitSemaphore--->error\n");
		return;
	}*/
	OS_SemWait(uSemaphore);
	//DEBUGPF("CTIOS_WaitSemaphore  OK!!! ---------------id[0x%x]\n\n",uSemaphore);
}

VOID CTIOS_TaskDelay(INT iTimeInMilliSeconds)
{
	OS_TaskDelay(iTimeInMilliSeconds);
}

BYTE CTIOS_SendMessage(CTI_MailBoxOfTask_t uMailbox, CTI_OsMessage_t *pstMsg)
{
	int  i = 0;
	U32  os_err;

//	DEBUGPF("\n\nTRDRV_OS_SendMessage--------wait......taskid[%x]\n",uMailbox);
//	OS_SemWait(msg_protect_sem);
	
	//pbierror("--------------------TRDRV_OS_SendMessage------------------start----task id [%x]\n",tTaskId);
	if(( pstMsg == NULL)||(uMailbox ==0))
	{
		pbierror("%s  param error \n",__FUNCTION__);
	//	OS_SemSignal(msg_protect_sem);
		return 1;
	}

	for(i =0;i<MAX_CAS_TASK_NUM;i++)
	{
		if((g_ctica_task_info[i].task_id == uMailbox)&&(g_ctica_task_info[i].msgque_id != 0xffffffff))
		{
			break;
		}
	}

	if( i == MAX_CAS_TASK_NUM)
	{
		pbierror("%s   error  line %d \n",__FUNCTION__,__LINE__);
	//	OS_SemSignal(msg_protect_sem);
		return 1;
	}
	
	os_err = OS_QueueSend(g_ctica_task_info[i].msgque_id, pstMsg, sizeof(CTI_OsMessage_t),200, 0);
	if(os_err != OS_SUCCESS)
	{
		pbierror("TRDRV_OS_SendMessage: send message to queue failed.---[0x%x]\n",os_err);
	//	OS_SemSignal(msg_protect_sem);
		return 1;
	}
	
	//DEBUGPF("---------------------------TRDRV_OS_SendMessage------------------ok\n");
	//OS_SemSignal(msg_protect_sem);
	return 0;

}
CTI_OsMessage_t * CTIOS_ReceiveMessage(CTI_MailBoxOfTask_t uMailbox)
{
	U32  i;
	CTI_OsMessage_t *msg_temp  = NULL; /*这个msg buf 如何释放呢????????????????*/
	U32  os_err;
	U32  actual_size = 0;
	
	//OS_SemWait(msg_protect_sem);

//	DEBUGPF("TRDRV_OS_ReceiveMessage------------------start task id [%x]\n",tTaskId);
	if(uMailbox ==0)
	{
		pbierror("%s  param error  \n",__FUNCTION__);
	//	OS_SemSignal(msg_protect_sem);
		return NULL;
	}

	for(i =0;i<MAX_CAS_TASK_NUM;i++)
	{
		if((g_ctica_task_info[i].task_id == uMailbox)&&(g_ctica_task_info[i].msgque_id != 0xffffffff))
		{
			break;
		}
	}
	//printf("%s   line %d\n",__FUNCTION__,__LINE__);
	if( i == MAX_CAS_TASK_NUM)
	{
		pbierror("%s   error  line %d   id %x\n\n",__FUNCTION__,__LINE__,uMailbox);
	//	OS_SemSignal(msg_protect_sem);
		return NULL;
	}
	// printf("%s   line %d\n",__FUNCTION__,__LINE__);
	msg_temp =(CTI_OsMessage_t *)OS_MemAllocate(system_p, sizeof(CTI_OsMessage_t));
	if(msg_temp == NULL)
	{
		pbierror("%s   error  line %d \n",__FUNCTION__,__LINE__);
		//OS_SemSignal(msg_protect_sem);
		return NULL;
	}
	//printf("%s   line %d\n",__FUNCTION__,__LINE__);
	os_err = OS_QueueReceive(g_ctica_task_info[i].msgque_id, msg_temp, sizeof(CTI_OsMessage_t),&actual_size, -1 );
	if( os_err < 0 )
	{
//		pbierror("%s   error  line %d \n",__FUNCTION__,__LINE__);
		OS_MemDeallocate(system_p,msg_temp);
	//	OS_SemSignal(msg_protect_sem);
		return NULL;
	}
	if(actual_size>0)
	{
		DEBUGPF("TRDRV_OS_ReceiveMessage-----------------tasid %x -actualsize %d   ok\n",uMailbox,actual_size);
	}
	else
	{
	//	pbierror("%s   error  line %d \n",__FUNCTION__,__LINE__);
		OS_MemDeallocate(system_p,msg_temp);
		msg_temp =NULL;
	}
	//OS_SemSignal(msg_protect_sem);

	return (CTI_OsMessage_t*)msg_temp;


}

int IsDigit(int ch) { return ch>='0' && ch<='9'; }
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
#ifndef TSIZEOF
#define TSIZEOF(name)	(sizeof(name)/sizeof(char))
#endif
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

VOID CTIOS_Print(CHAR *Format_p, ...)
{
	int ret;
	char s[1024];

	s[0]=0;
	
	va_list args;
	va_start(args,Format_p);
	ret = vprintf(Format_p,args);
	vstprintf_s(s+strlen(s),TSIZEOF(s)-strlen(s), Format_p, args);	
	va_end(args);
	__android_log_write(ANDROID_LOG_INFO,"CTICA",s);
	//fflush(stdout);
	

}

#include <sys/types.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include<errno.h>

static unsigned char IP_Buffer[255];
#define CA_PORT		8881
static unsigned char IP_Address[4];
//#define	BROADCAST255255255255

unsigned char * CTICASTB_GetAddress()
{
	return IP_Address;
}

void CTICAclient_GetSTBipAddr(BYTE* ipaddr)
{
	int s;
       struct ifconf conf;
       struct ifreq *ifr;
       char buff[BUFSIZ];
       int num;
       int i;

       s = socket(PF_INET, SOCK_DGRAM, 0);
       conf.ifc_len = BUFSIZ;
       conf.ifc_buf = buff;

       ioctl(s, SIOCGIFCONF, &conf);
       num = conf.ifc_len / sizeof(struct ifreq);
       ifr = conf.ifc_req;
	memset((void*)IP_Address,0,4);
       for(i=0;i < num;i++)
       {
               struct sockaddr_in *sin = (struct sockaddr_in *)(&ifr->ifr_addr);

               ioctl(s, SIOCGIFFLAGS, ifr);
               if(((ifr->ifr_flags & IFF_LOOPBACK) == 0) && (ifr->ifr_flags & IFF_UP))
               {
                       printf("%s (%s)\n",
                               ifr->ifr_name,
                               inet_ntoa(sin->sin_addr));
                       memcpy((void*)ipaddr,(void*)&sin->sin_addr,4);	//support ipv4 only
                       memcpy((void*)IP_Address,(void*)&sin->sin_addr,4);	//support ipv4 only
                       pbiinfo("ip address %d,%d,%d,%d\n",ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]);
					   ipaddr[4] = (unsigned char)(CA_PORT>>8);
					   ipaddr[5] = (unsigned char)(CA_PORT);

               }
               ifr++;
       }	
       close(s);
}

BOOL CTI_Client_SendData(BYTE* Ipaddr,U16 port,BYTE *data,int length)
{
	#if 0
	int    sockfd, n;
    char    recvline[4096], sendline[4096];
    struct sockaddr_in    servaddr;

    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
	    pbiinfo("create socket error: %s(errno: %d)\n", strerror(errno),errno);
	    return FALSE;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    if( inet_pton(AF_INET, Ipaddr, &servaddr.sin_addr) <= 0)
    {
	    pbiinfo("inet_pton error for %s\n",Ipaddr);
    	close(sockfd);
	   return FALSE;
    }

    if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
   	 pbiinfo("connect error: %s(errno: %d)\n",strerror(errno),errno);
    close(sockfd);
	  return FALSE;
    }

    pbiinfo("send msg to server: %s, port\n",Ipaddr,port);
    
    if( send(sockfd, data, len, 0) < 0)
    {
    close(sockfd);
    pbiinfo("send msg error: %s(errno: %d)\n", strerror(errno), errno);
   return FALSE;  
    }

    close(sockfd);
	return TRUE;	
#else
	int sockfd,ret;
	int size,len;
	int so_broadcast = 1;
	struct sockaddr_in my_con;
	struct sockaddr_in cl_con;

	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd<0)
	{
	    pbiinfo("create socket error: %s(errno: %d)\n", strerror(errno),errno);
	    return FALSE;
	}
	port = 8881;
	my_con.sin_family = AF_INET;
	my_con.sin_port = ntohs(port);
	my_con.sin_addr.s_addr = inet_addr(Ipaddr);
//	my_con.sin_addr.s_addr = htonl(INADDR_ANY);

	len = sizeof(struct sockaddr);

	printf("send data to server .........\n");
	pbiinfo("send data to server .........\n");
	
	ret = sendto(sockfd,data,length,0,(struct sockaddr*)&my_con,len);
	if(ret < 0)
	{
	    pbiinfo("sendto socket error: %s(errno: %d)\n", strerror(errno),errno);
		printf("sendto socket error: %s(errno: %d)\n", strerror(errno),errno);
	    close(sockfd);
		return FALSE;
	}
    close(sockfd);
	return TRUE;	

#endif
}

BOOL CTI_Client_SendDataBroadcast(U16 port,BYTE *data,int length)
{
	int sockfd,ret;
	int size,len;
	int so_broadcast = 1;
	unsigned char buffer[50];
	struct sockaddr_in my_con;
	struct sockaddr_in cl_con;

	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd<0)
	{
	    pbiinfo("create socket error: %s(errno: %d)\n", strerror(errno),errno);
	    return FALSE;
	}
	
	port = CA_PORT;
	my_con.sin_family = AF_INET;
	my_con.sin_port = ntohs(port);
#ifdef	BROADCAST255255255255
	my_con.sin_addr.s_addr = inet_addr("255.255.255.255");
#else
	IP_Address[3] = 0xFF;
	sprintf(buffer,"%d.%d.%d.%d",IP_Address[0],IP_Address[1],IP_Address[2],IP_Address[3]);
	my_con.sin_addr.s_addr = inet_addr(buffer);
#endif

	len = sizeof(struct sockaddr);

	setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,&so_broadcast,sizeof(so_broadcast));

	printf("send data to server .........\n");
	pbiinfo("send data to server .........\n");
	
	ret = sendto(sockfd,data,length,0,(struct sockaddr*)&my_con,len);
	if(ret < 0)
	{
	    pbiinfo("sendto socket error: %s(errno: %d)\n", strerror(errno),errno);
		printf("sendto socket error: %s(errno: %d)\n", strerror(errno),errno);
	    close(sockfd);
		return FALSE;
	}
    close(sockfd);
	return TRUE;	
	
}

extern void CTICA_CLIENT_SEND_CA_MESSAGE_QUERY( void* param, unsigned int arg1 );

BOOL CTI_Client_ReciveDataBroadcast(void *arg)
{
	int sockfd,ret;
	int size,len;
	int so_broadcast = 1;
	unsigned short port;
	struct sockaddr_in my_con;
	struct sockaddr_in c_addr;
	int addr_len;
	int i = 0;
	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd<0)
	{
	    pbiinfo("create socket error: %s(errno: %d)\n", strerror(errno),errno);
	    return FALSE;
	}
	port = CA_PORT;
	my_con.sin_family = AF_INET;
	my_con.sin_port = ntohs(port);
	my_con.sin_addr.s_addr = htonl(INADDR_ANY);
	len = sizeof(my_con);
	
	if(bind(sockfd,(struct sockaddr*)&my_con,len)<0)
	{
	    pbiinfo("bind socket error: %s(errno: %d)\n", strerror(errno),errno);
	    close(sockfd);
		return FALSE;
	}
	
 	addr_len = sizeof(c_addr);
	pbiinfo("server read to receive......\n");
	printf("server read to receive......\n");
	
 	while (1)     
	{         
		len = recvfrom(sockfd, IP_Buffer, sizeof(IP_Buffer) - 1, 0,(struct sockaddr *) &c_addr, &addr_len); 
		if (len < 0)         
		{             
		    pbiinfo("recvfrom error: %s(errno: %d)\n", strerror(errno),errno);
			printf("recvfrom error: %s(errno: %d)\n", strerror(errno),errno);
		    close(sockfd);
			return FALSE;
		}  
		IP_Buffer[len] = '\0';    
		

		pbiinfo("receive CADATA%s:%d data\n\r",inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port) );     
		printf("receive CADATA%s:%d data\n\r",inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port) );  
		pbiinfo("ip data =%d: \n",len);
	//	for(i=0; i<len; i++)
	//	{
	//		pbiinfo("%02x ", IP_Buffer[i]);
	//	}
	//	pbiinfo("\n");
		CTICAS_SendMsgToCAcore(CA_MESSAGE_QUERY, IP_Buffer,len);
	} 
	printf("CTI_Client_ReciveDataBroadcast exit......\n");

	return TRUE;
}

