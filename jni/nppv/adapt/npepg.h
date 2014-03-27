#ifndef _NPEPG_H_
#define _NPEPG_H_


#ifdef __cplusplus
extern "C"
{
#endif

    int NpEpg_Init(void);
    int NpEpg_Term(void);
    int NpEpg_Start(void);
    int NpEpg_Stop(void);
    int NpEpg_Pause(void);
    int NpEpg_Resume(void);
    char *NpEpg_GetPF(int serv_id, int ts_id, int on_id, int flag);
    void NpEpg_Retstr_Free(void);
	int NpEpg_Debug(int flag);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* #ifndef _NPEPG_H_ */

