/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : 
* Description : 
* Author : zyliu
* History :
*	2011/9/30 : Initial Version
******************************************************************************/
#ifndef __SYS_DEBUG_H
#define __SYS_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************/
/*              Includes											*/
/*******************************************************/

/*******************************************************/
/*              Exported Defines and Macros            */
/*******************************************************/
#define SAVE_TO_LOG_FILE	1


#define MODULE_DAL			0
#define MODULE_DAL_CORE		1
#define MODULE_CL			2
#define MODULE_SYSTIME		3
#define MODULE_MPLAYER		4
#define MODULE_JSPLAY		5
#define MODULE_UPDATE		6
#define MODULE_STILL            7
#define MODULE_DIALOG           8

#define	SD_DEBUG			4
#define	SD_WARNING			3
#define	SD_ERROR			2
#define	SD_FATAL_ERR		1

extern unsigned int g_sys_debug_flag;
extern unsigned int g_sys_debug_level;
extern FILE*		g_log_fp;


#define SYS_DEBUG(l,m,fmt,args...)			{\
												if( g_sys_debug_flag & (1 << m) && g_sys_debug_level >= l)\
												{\
													if(SD_DEBUG == g_sys_debug_level)\
														printf(">>>>>");\
													if(SD_WARNING == g_sys_debug_level)\
														printf("!!!!!");\
													if(SD_ERROR == g_sys_debug_level)\
														printf("?????");\
													if(SD_FATAL_ERR == g_sys_debug_level)\
														printf("????? ????? ????? ?????");\
													printf("[%s][%s][%s]  ", level_string(l),module_string(m),__FUNCTION__);\
													printf( fmt, ##args);\
													printf("\n");\
													if( NULL != g_log_fp)\
													{\
														if(SD_DEBUG == g_sys_debug_level)\
															fprintf(g_log_fp,">>>>>");\
														if(SD_WARNING == g_sys_debug_level)\
															fprintf(g_log_fp,"!!!!!");\
														if(SD_ERROR == g_sys_debug_level)\
															fprintf(g_log_fp,"?????");\
														if(SD_FATAL_ERR == g_sys_debug_level)\
															fprintf(g_log_fp,"????? ????? ????? ?????");\
														fprintf(g_log_fp,"[%s][%s][%s]  ", level_string(l),module_string(m),__FUNCTION__);\
														fprintf(g_log_fp, fmt, ##args);\
														fprintf(g_log_fp,"\n");\
													}\
													system("sync");\
												}\
											}


#define SYS_TRACE(m)
											/*{\
												if( g_sys_debug_flag & (1 << m))\
												{\
													printf("[%s][%s] entry\n", module_string(m),__FUNCTION__);\
												}\
											}\
											*/

/*******************************************************/
/*              Exported Types			                */
/*******************************************************/

/*******************************************************/
/*              Exported Variables		                */
/*******************************************************/

/*******************************************************/
/*              External				                   */
/*******************************************************/

/*******************************************************/
/*              Exported Functions		                */
/*******************************************************/

int sys_debug_init(void);

int sys_debug_clear_module(void);
int sys_debug_add_module(int module);
int sys_debug_del_module(int module);

char* module_string(int module);
char* level_string(int level);


#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __XXXXXXX_H */
/* End of XXXXXXXX.h  --------------------------------------------------------- */

