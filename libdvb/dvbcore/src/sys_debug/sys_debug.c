

#include <stdio.h>
#include <string.h>
#include <sys_debug.h>




unsigned int g_sys_debug_flag = 0;
unsigned int g_sys_debug_level = 0;


FILE* g_log_fp = NULL;
#define LOG_FILE	"./sys_log.dat"


int sys_debug_init(void)
{
#if SAVE_TO_LOG_FILE
	g_log_fp = fopen(LOG_FILE,"w");
	if( NULL == g_log_fp){
		printf(" ********************************        log file %s open failed!\n", LOG_FILE);
	}
#endif

/*
#define MODULE_DAL			0
#define MODULE_DAL_CORE	1
#define MODULE_CL			2
#define MODULE_SYSTIME		3
#define MODULE_MPLAYER		4
#define MODULE_JSPLAY		5
#define MODULE_UPDATE		6
#define MODULE_STILL		7
#define MODULE_DIALOG		8

*/

	sys_debug_add_module(MODULE_DAL);
	sys_debug_add_module(MODULE_DAL_CORE);
	sys_debug_add_module(MODULE_CL);
	sys_debug_add_module(MODULE_SYSTIME);
	sys_debug_add_module(MODULE_MPLAYER);
	sys_debug_add_module(MODULE_JSPLAY);
	sys_debug_add_module(MODULE_UPDATE);
	sys_debug_add_module(MODULE_STILL);
	sys_debug_add_module(MODULE_DIALOG);
	
	
	
	g_sys_debug_level = SD_DEBUG;
	return 0;
}

int sys_debug_clear_module(void)
{
	g_sys_debug_flag = 0;
	return 0;
}

int sys_debug_add_module(int module)
{
	g_sys_debug_flag |= 1 << module;
	return 0;
}

int sys_debug_del_module(int module)
{
	g_sys_debug_flag &= ~(1 << module);
	return 0;
}

int sys_debug_set_level(int level)
{
	g_sys_debug_level = level;
	return 0;
}


char* module_string(int module)
{
#define E(e) case e: return #e
	switch( module)
	{
		E(MODULE_DAL);
		E(MODULE_DAL_CORE);
		E(MODULE_CL);
		E(MODULE_SYSTIME);
		E(MODULE_MPLAYER);
		E(MODULE_JSPLAY);
		E(MODULE_UPDATE);
		E(MODULE_STILL);
		E(MODULE_DIALOG);
		default: return "unknow module";
	}	
}

char* level_string(int level)
{	
#define E(e) case e: return #e
	switch( level)
	{
		E(SD_DEBUG);
		E(SD_WARNING);
		E(SD_ERROR);
		E(SD_FATAL_ERR);
		default: return "unknow level";
	}
}

void sys_debug_test(void)
{
	sys_debug_set_level(SD_DEBUG);

	SYS_DEBUG(SD_DEBUG, MODULE_CL, "SD_DEBUG");
	SYS_DEBUG(SD_WARNING, MODULE_CL, "SD_WARNING");
	SYS_DEBUG(SD_ERROR, MODULE_CL, "SD_ERROR");
	SYS_DEBUG(SD_FATAL_ERR, MODULE_CL, "SD_FATAL_ERR");
	printf("\n");

	sys_debug_set_level(SD_WARNING);
	SYS_DEBUG(SD_DEBUG, MODULE_CL, "SD_DEBUG");
	SYS_DEBUG(SD_WARNING, MODULE_CL, "SD_WARNING");
	SYS_DEBUG(SD_ERROR, MODULE_CL, "SD_ERROR");
	SYS_DEBUG(SD_FATAL_ERR, MODULE_CL, "SD_FATAL_ERR");
	printf("\n");

	sys_debug_set_level(SD_ERROR);
	SYS_DEBUG(SD_DEBUG, MODULE_CL, "SD_DEBUG");
	SYS_DEBUG(SD_WARNING, MODULE_CL, "SD_WARNING");
	SYS_DEBUG(SD_ERROR, MODULE_CL, "SD_ERROR");
	SYS_DEBUG(SD_FATAL_ERR, MODULE_CL, "SD_FATAL_ERR");
	printf("\n");

	sys_debug_set_level(SD_FATAL_ERR);
	SYS_DEBUG(SD_DEBUG, MODULE_CL, "SD_DEBUG");
	SYS_DEBUG(SD_WARNING, MODULE_CL, "SD_WARNING");
	SYS_DEBUG(SD_ERROR, MODULE_CL, "SD_ERROR");
	SYS_DEBUG(SD_FATAL_ERR, MODULE_CL, "SD_FATAL_ERR");
	printf("\n");
}

