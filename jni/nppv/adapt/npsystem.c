
#include "log/anpdebug.h"


#include "dvbplayer.h"
#include "ca.h"


static int system_init = 0;

int NpSystem_Init(void)
{
    DebugMessage("=========   PXWANG	CALL BY 	0701	%s", __FUNCTION__);
    if(system_init == 0)
    {

       // Pvware_DRV_Init();
        Code_Init();
		NpSystem_set_debug(1);  
		NpHLS_Init();
		//NpSystem_Create(); 
		system_init = 1;
    }
	DrvPlayerInit(1);
	DVBPlayer_Init(NULL);
	
    return 11;
}




static int32_t guSys_Task = 0xFFFFFFFF;
static const int32_t guSys_Stack_Size = 18 * 1024;		/* 18K */
static const int32_t guSys_Priority = 4;

static void _system_init_task(void);

void NpSystem_Create(void)
{
    DebugMessage("=========   PXWANG	CALL BY 		0701	%s", __FUNCTION__);
    OS_TaskCreate(&guSys_Task, "SysInit_Task", (void * ( *)(void *))_system_init_task, NULL, NULL, guSys_Stack_Size, guSys_Priority, 0);

}
static void _system_init_task(void)
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    DVBTable_Init();
    CA_Init();
    DrvPlayerInit(1);
    DVBPlayer_Init(NULL);
    DVBPlayer_RegCACallback(CA_CallBack);
}


int NpSystem_set_charset(int charset_id)
{
    Code_Set_Default(charset_id);
    return 0;
}


int NpSystem_set_debug(int debug_val)
{
    DebugMessage_Switch(debug_val);
    return 0;
}








